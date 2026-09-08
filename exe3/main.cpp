#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>

using namespace std;

struct Job {
    pid_t pid;
    string command;
    bool is_running;
};

vector<Job> background_jobs;

void update_jobs() {
    for (auto& job : background_jobs) {
        if (!job.is_running) continue;
        int status;
        pid_t result = waitpid(job.pid, &status, WNOHANG);
        if (result > 0) {
            job.is_running = false;
        }
    }
}

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream token_stream(str);
    while (getline(token_stream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

string find_in_path(const string& command) {
    if (command.find('/') != string::npos) {
        return command;
    }
    char* path_env = getenv("PATH");
    if (!path_env) return "";

    string path_str(path_env);
    vector<string> dirs = split(path_str, ':');
    for (const auto& dir : dirs) {
        string full_path = dir + "/" + command;
        if (access(full_path.c_str(), X_OK) == 0) {
            return full_path;
        }
    }
    return "";
}

struct CommandInfo {
    vector<string> args;
    string input_file;
    string output_file;
    bool append_output = false;
};

CommandInfo parse_redirections(const vector<string>& tokens) {
    CommandInfo cmd;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "<" && i + 1 < tokens.size()) {
            cmd.input_file = tokens[i + 1];
            i++;
        } else if (tokens[i] == ">" && i + 1 < tokens.size()) {
            cmd.output_file = tokens[i + 1];
            cmd.append_output = false;
            i++;
        } else if (tokens[i] == ">>" && i + 1 < tokens.size()) {
            cmd.output_file = tokens[i + 1];
            cmd.append_output = true;
            i++;
        } else {
            cmd.args.push_back(tokens[i]);
        }
    }
    return cmd;
}

void execute_pipeline(const vector<string>& line_tokens, bool background) {
    vector<vector<string>> pipe_segments;
    vector<string> current_segment;
    
    for (const auto& token : line_tokens) {
        if (token == "|") {
            if (!current_segment.empty()) {
                pipe_segments.push_back(current_segment);
                current_segment.clear();
            }
        } else {
            current_segment.push_back(token);
        }
    }
    if (!current_segment.empty()) {
        pipe_segments.push_back(current_segment);
    }

    if (pipe_segments.empty()) return;

    size_t num_cmds = pipe_segments.size();
    vector<int> pipe_fds(2 * (num_cmds > 1 ? num_cmds - 1 : 0));

    for (size_t i = 0; i < num_cmds - 1; ++i) {
        if (pipe(pipe_fds.data() + i * 2) < 0) {
            perror("pipe failed");
            return;
        }
    }

    vector<pid_t> pids;
    bool is_builtin_single = (num_cmds == 1);
    CommandInfo single_cmd_info = parse_redirections(pipe_segments[0]);

    if (is_builtin_single && !single_cmd_info.args.empty()) {
        if (single_cmd_info.args[0] == "exit") {
            exit(0);
        } else if (single_cmd_info.args[0] == "cd") {
            string target_dir = (single_cmd_info.args.size() > 1) ? single_cmd_info.args[1] : getenv("HOME");
            if (chdir(target_dir.c_str()) == 0) {
                cout << "[cd worked]" << endl;
            } else {
                perror("cd failed");
            }
            return;
        } else if (single_cmd_info.args[0] == "pwd") {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                cout << cwd << endl;
            } else {
                perror("pwd failed");
            }
            return;
        } else if (single_cmd_info.args[0] == "myjobs") {
            for (const auto& job : background_jobs) {
                cout << "[" << job.pid << "] " << job.command << (job.is_running ? " RUNING" : " DONE") << endl;
            }
            return;
        }
    }

    for (size_t i = 0; i < num_cmds; ++i) {
        CommandInfo cmd_info = parse_redirections(pipe_segments[i]);
        if (cmd_info.args.empty()) continue;

        string executable = find_in_path(cmd_info.args[0]);
        if (executable.empty()) {
            cerr << "Command not found: " << cmd_info.args[0] << endl;
            return;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return;
        } else if (pid == 0) {
            if (i > 0) {
                dup2(pipe_fds[(i - 1) * 2], STDIN_FILENO);
            } else if (!cmd_info.input_file.empty()) {
                int fd_in = open(cmd_info.input_file.c_str(), O_RDONLY);
                if (fd_in < 0) {
                    perror("open input file failed");
                    _exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            if (i < num_cmds - 1) {
                dup2(pipe_fds[i * 2 + 1], STDOUT_FILENO);
            } else if (!cmd_info.output_file.empty()) {
                int flags = O_WRONLY | O_CREAT;
                if (cmd_info.append_output) flags |= O_APPEND;
                else flags |= O_TRUNC;
                
                int fd_out = open(cmd_info.output_file.c_str(), flags, 0644);
                if (fd_out < 0) {
                    perror("open output file failed");
                    _exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }

            for (int fd : pipe_fds) {
                close(fd);
            }

            vector<char*> c_args;
            for (auto& arg : cmd_info.args) {
                c_args.push_back(&arg[0]);
            }
            c_args.push_back(nullptr);

            execve(executable.c_str(), c_args.data(), environ);
            perror("execve failed");
            _exit(1);
        } else {
            pids.push_back(pid);
        }
    }

    for (int fd : pipe_fds) {
        close(fd);
    }

    if (background) {
        background_jobs.push_back({pids.back(), line_tokens[0], true});
    } else {
        for (pid_t p : pids) {
            int status;
            waitpid(p, &status, 0);
        }
    }
}

int main() {
    string line;
    while (true) {
        cout << "$ ";
        if (!getline(cin, line)) break;
        if (line.empty()) continue;

        update_jobs();

        bool background = false;
        if (line.back() == '&') {
            background = true;
            line.pop_back();
            while (!line.empty() && (line.back() == ' ' || line.back() == '\t')) {
                line.pop_back();
            }
        }

        vector<string> tokens = split(line, ' ');
        if (tokens.empty()) continue;

        execute_pipeline(tokens, background);
    }
    return 0;
}
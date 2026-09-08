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
#include <map>

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

        vector<string> args = split(line, ' ');
        if (args.empty()) continue;

        if (args[0] == "exit") {
            break;
        } else if (args[0] == "cd") {
            string target_dir = (args.size() > 1) ? args[1] :getenv("HOME");
            if (chdir(target_dir.c_str()) == 0) {
                cout << "[cd worked]" << endl;
            } else {
                perror("cd failed");
            }
            continue;
        } else if (args[0] == "pwd") {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                cout << cwd << endl;
            } else {
                perror("pwd failed");
            }
            continue;
        } else if (args[0] == "myjobs") {
            for (const auto& job : background_jobs) {
                cout << "[" << job.pid << "] " << job.command << (job.is_running ? " RUNING" : " DONE") << endl;
            }
            continue;
        }

        string executable = find_in_path(args[0]);
        if (executable.empty()) {
            cerr << "Command not found: " << args[0] << endl;
            continue;
        }

        vector<char*> c_args;
        for (auto& arg : args) {
            c_args.push_back(&arg[0]);
        }
        c_args.push_back(nullptr);

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            continue;
        } else if (pid == 0) {
            execve(executable.c_str(), c_args.data(), environ);
            perror("execve failed");
            _exit(1);
        } else {
            if (background) {
                background_jobs.push_back({pid, line, true});
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }
    return 0;
}
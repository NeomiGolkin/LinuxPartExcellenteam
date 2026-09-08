#include "myfs.h"
#include <string.h>
#include <iostream>
#include <stdexcept>
#include <algorithm>

const char *MyFs::MYFS_MAGIC = "MYFS";

MyFs::MyFs(BlockDeviceSimulator *blkdevsim_) : blkdevsim(blkdevsim_) {
    blkdevsim->read(0, sizeof(header), (char *)&header);

    if (strncmp(header.magic, MYFS_MAGIC, 4) != 0 || header.version != CURR_VERSION) {
        std::cout << "Did not find myfs instance on blkdev" << std::endl;
        std::cout << "Creating..." << std::endl;
        format();
        std::cout << "Finished!" << std::endl;
    }
}

void MyFs::format() {
    memset(&header, 0, sizeof(header));
    strncpy(header.magic, MYFS_MAGIC, 4);
    header.version = CURR_VERSION;
    for (int i = 0; i < MAX_FILES; ++i) header.files[i].in_use = false;
    for (int i = 0; i < MAX_BLOCKS; ++i) header.fat[i] = FAT_FREE;
    save_header();
}

void MyFs::save_header() {
    blkdevsim->write(0, sizeof(header), (const char*)&header);
}

int MyFs::find_file(const std::string& path_str) {
    for (int i = 0; i < MAX_FILES; ++i) {
        if (header.files[i].in_use && std::string(header.files[i].name) == path_str) {
            return i;
        }
    }
    return -1;
}

uint16_t MyFs::allocate_block() {
    for (uint16_t i = 1; i < MAX_BLOCKS; ++i) {
        if (header.fat[i] == FAT_FREE) {
            header.fat[i] = FAT_EOF;
            return i;
        }
    }
    throw std::runtime_error("Disk is full");
}

void MyFs::free_blocks(uint16_t start_block) {
    uint16_t curr = start_block;
    while (curr != FAT_FREE && curr != FAT_EOF) {
        uint16_t next = header.fat[curr];
        header.fat[curr] = FAT_FREE;
        curr = next;
    }
}

void MyFs::create_file(std::string path_str, bool directory) {
    if (path_str.length() >= 24) throw std::runtime_error("File name too long");
    if (find_file(path_str) != -1) throw std::runtime_error("File or directory already exists");
    
    // בדיקה האם תיקיית האב קיימת (אם רלוונטי)
    size_t slash_pos = path_str.find_last_of('/');
    if (slash_pos != std::string::npos) {
        std::string parent_dir = path_str.substr(0, slash_pos);
        int p_idx = find_file(parent_dir);
        if (p_idx == -1 || !header.files[p_idx].is_dir) {
            throw std::runtime_error("Parent directory does not exist");
        }
    }

    int empty_idx = -1;
    for (int i = 0; i < MAX_FILES; ++i) {
        if (!header.files[i].in_use) {
            empty_idx = i;
            break;
        }
    }
    if (empty_idx == -1) throw std::runtime_error("No free inodes");

    strncpy(header.files[empty_idx].name, path_str.c_str(), 23);
    header.files[empty_idx].name[23] = '\0';
    header.files[empty_idx].in_use = true;
    header.files[empty_idx].is_dir = directory;
    header.files[empty_idx].size = 0;
    header.files[empty_idx].first_block = FAT_FREE;

    save_header();
}

void MyFs::remove_file(std::string path_str) {
    int idx = find_file(path_str);
    if (idx == -1) throw std::runtime_error("File or directory not found");
    
    if (header.files[idx].is_dir) {
        for (int i = 0; i < MAX_FILES; ++i) {
            if (header.files[i].in_use) {
                std::string fname(header.files[i].name);
                if (fname.find(path_str + "/") == 0) {
                    throw std::runtime_error("Directory is not empty");
                }
            }
        }
    }

    free_blocks(header.files[idx].first_block);
    header.files[idx].in_use = false;
    save_header();
}

void MyFs::rename_file(std::string old_path, std::string new_path) {
    int idx = find_file(old_path);
    if (idx == -1) throw std::runtime_error("Source not found");
    if (find_file(new_path) != -1) throw std::runtime_error("Destination already exists");
    if (new_path.length() >= 24) throw std::runtime_error("New name too long");

    strncpy(header.files[idx].name, new_path.c_str(), 23);
    header.files[idx].name[23] = '\0';
    
    // עדכון שמות תתי-הקבצים אם זו תיקייה
    if (header.files[idx].is_dir) {
        std::string old_prefix = old_path + "/";
        std::string new_prefix = new_path + "/";
        for (int i = 0; i < MAX_FILES; ++i) {
            if (header.files[i].in_use) {
                std::string child_name(header.files[i].name);
                if (child_name.find(old_prefix) == 0) {
                    std::string updated = new_prefix + child_name.substr(old_prefix.length());
                    strncpy(header.files[i].name, updated.c_str(), 23);
                    header.files[i].name[23] = '\0';
                }
            }
        }
    }
    save_header();
}

std::string MyFs::get_content(std::string path_str) {
    int idx = find_file(path_str);
    if (idx == -1) throw std::runtime_error("File not found");
    if (header.files[idx].is_dir) throw std::runtime_error("Cannot read directory content");

    uint32_t size = header.files[idx].size;
    if (size == 0) return "";

    std::string content = "";
    uint16_t curr = header.files[idx].first_block;
    uint32_t read_so_far = 0;

    char buf[BLOCK_SIZE];
    while (curr != FAT_FREE && curr != FAT_EOF && read_so_far < size) {
        blkdevsim->read(DATA_OFFSET + curr * BLOCK_SIZE, BLOCK_SIZE, buf);
        uint32_t to_read = std::min((uint32_t)BLOCK_SIZE, size - read_so_far);
        content.append(buf, to_read);
        read_so_far += to_read;
        curr = header.fat[curr];
    }
    return content;
}

void MyFs::set_content(std::string path_str, std::string content) {
    int idx = find_file(path_str);
    if (idx == -1) throw std::runtime_error("File not found");
    if (header.files[idx].is_dir) throw std::runtime_error("Cannot edit a directory");

    free_blocks(header.files[idx].first_block);
    header.files[idx].first_block = FAT_FREE;
    header.files[idx].size = content.length();

    if (content.length() == 0) {
        save_header();
        return;
    }

    uint16_t curr = allocate_block();
    header.files[idx].first_block = curr;

    uint32_t written = 0;
    while (written < content.length()) {
        uint32_t to_write = std::min((uint32_t)BLOCK_SIZE, (uint32_t)(content.length() - written));
        char buf[BLOCK_SIZE];
        memset(buf, 0, BLOCK_SIZE);
        memcpy(buf, content.c_str() + written, to_write);
        blkdevsim->write(DATA_OFFSET + curr * BLOCK_SIZE, BLOCK_SIZE, buf);
        written += to_write;

        if (written < content.length()) {
            uint16_t next = allocate_block();
            header.fat[curr] = next;
            curr = next;
        }
    }
    save_header();
}

std::vector<std::string> MyFs::list_dir(std::string path_str) {
    std::vector<std::string> ans;
    
    if (!path_str.empty()) {
        int idx = find_file(path_str);
        if (idx == -1 || !header.files[idx].is_dir) {
            throw std::runtime_error("Directory not found");
        }
    }

    std::string prefix = path_str.empty() ? "" : path_str + "/";
    
    for (int i = 0; i < MAX_FILES; ++i) {
        if (header.files[i].in_use) {
            std::string fname(header.files[i].name);
            
            if (path_str.empty()) {
                if (fname.find('/') == std::string::npos) {
                    ans.push_back(fname + (header.files[i].is_dir ? " (dir)" : " \t" + std::to_string(header.files[i].size)));
                }
            } else {
                if (fname.find(prefix) == 0 && fname.length() > prefix.length()) {
                    std::string rest = fname.substr(prefix.length());
                    if (rest.find('/') == std::string::npos) { 
                        ans.push_back(rest + (header.files[i].is_dir ? " (dir)" : " \t" + std::to_string(header.files[i].size)));
                    }
                }
            }
        }
    }
    return ans;
}
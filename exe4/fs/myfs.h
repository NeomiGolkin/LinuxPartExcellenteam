#ifndef __MYFS_H__
#define __MYFS_H__

#include <memory>
#include <vector>
#include <string>
#include <stdint.h>
#include "blkdev.h"

const int MAX_FILES = 100;
const int MAX_BLOCKS = 1000;
const int BLOCK_SIZE = 1024;
const int DATA_OFFSET = 10240; // הסטה שמייצגת את תחילת אזור המידע
const uint16_t FAT_EOF = 0xFFFF;
const uint16_t FAT_FREE = 0;

struct FileDesc {
    char name[24];
    bool in_use;
    bool is_dir;
    uint32_t size;
    uint16_t first_block;
};

class MyFs {
public:
    MyFs(BlockDeviceSimulator *blkdevsim_);
    void format();
    void create_file(std::string path_str, bool directory);
    std::string get_content(std::string path_str);
    void set_content(std::string path_str, std::string content);
    
    // שונה להחזרת וקטור בהתאם להערות השלד
    std::vector<std::string> list_dir(std::string path_str);
    
    // פונקציות חדשות עבור דרישות ההסרה והעברה (חלק ב')
    void remove_file(std::string path_str);
    void rename_file(std::string old_path, std::string new_path);

private:
    struct myfs_header {
        char magic[4];
        uint8_t version;
        FileDesc files[MAX_FILES];
        uint16_t fat[MAX_BLOCKS];
    };

    BlockDeviceSimulator *blkdevsim;
    myfs_header header;

    void save_header(); 
    int find_file(const std::string& path_str);
    uint16_t allocate_block();
    void free_blocks(uint16_t start_block);

    static const uint8_t CURR_VERSION = 0x03;
    static const char *MYFS_MAGIC;
};

#endif // __MYFS_H__
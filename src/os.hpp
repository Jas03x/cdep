#ifndef OS_HPP
#define OS_HPP

#include <stdint.h>

namespace OS
{
    struct FileInfo
    {
        uint64_t ID;
        uint64_t create_time;
        uint64_t access_time;
        uint64_t write_time;
        bool exists;
    };

    bool GetFileInfo(const char* path, FileInfo& info);
    bool UpdateFileInfo(const char* path, const FileInfo& params);
}

#endif // OS_HPP
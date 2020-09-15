#ifndef FILE_HPP
#define FILE_HPP

#include <cstdio>
#include <cstdint>

struct File
{
    FILE* handle;

    enum
    {
        READ  = 0,
        WRITE = 1,
        MAX = 2
    };

    File();
    File(const char* path, uint32_t mode);
    ~File();

    File& operator=(File& other);
    
    bool is_open();
};

#endif // FILE_HPP
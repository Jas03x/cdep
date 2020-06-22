#ifndef FILE_HPP
#define FILE_HPP

#include <stdio.h>

#include <vector>

class File
{
private:
    FILE* m_file;

public:
    File(const char* path);
    ~File();
    
    bool is_open();
    bool is_eof();
    char getc();
    
    void skip_line();
    void skip_space();
    void read_token(std::vector<char>& buffer);
    bool read_include(std::vector<char>& buffer);
};

#endif // FILE_HPP
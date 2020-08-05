#ifndef FILE_HPP
#define FILE_HPP

#include <cstdio>

#include <vector>

class File
{
private:
    std::FILE* m_file;

public:
    File(const char* path);
    ~File();
    
    bool is_open();
    bool is_eof();

    char getc();
    bool ungetc(char c);
    
    void skip_line();
    void skip_space();
};

#endif // FILE_HPP
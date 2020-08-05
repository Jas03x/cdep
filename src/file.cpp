#include "file.hpp"

#include "ascii.hpp"

File::File(const char* path)
{
    m_file = std::fopen(path, "r");
}

File::~File()
{
    if(m_file != nullptr)
    {
        std::fclose(m_file);
        m_file = nullptr;
    }
}

bool File::is_open()      { return m_file != nullptr;        }
bool File::is_eof()       { return std::feof(m_file);             }
char File::getc()         { return std::fgetc(m_file);            }
bool File::ungetc(char c) { return std::ungetc(c, m_file) != EOF; }

void File::skip_line()
{
    while(true)
    {
        char c = std::fgetc(m_file);
        if((c == EOF) || (c == '\n'))
        {
            break;
        }
    }
}

void File::skip_space()
{
    while(true)
    {
        char c = fgetc(m_file);
        if(!is_space(c))
        {
            if(c != EOF)
            {
                std::ungetc(c, m_file);
            }
            break;
        }
    }
}

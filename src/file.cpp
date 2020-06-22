#include "file.hpp"

#include "ascii.hpp"

File::File(const char* path)
{
    m_file = fopen(path, "r");
}

File::~File()
{
    if(m_file != nullptr)
    {
        fclose(m_file);
        m_file = nullptr;
    }
}

bool File::is_open() { return m_file != nullptr; }
bool File::is_eof()  { return feof(m_file);  }
char File::getc()    { return fgetc(m_file); }

void File::skip_line()
{
    while(true)
    {
        char c = fgetc(m_file);
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
                ungetc(c, m_file);
            }
            break;
        }
    }
}

void File::read_token(std::vector<char>& buffer)
{
    buffer.clear();
    while(true)
    {
        char c = fgetc(m_file);
        if(is_text(c))
        {
            buffer.push_back(c);
        }
        else
        {
            if(c != EOF)
            {
                ungetc(c, m_file);
            }
            break;
        }
    }
}

bool File::read_include(std::vector<char>& buffer)
{
    skip_space();
    buffer.clear();

    bool status = true;
    char s = fgetc(m_file);
    if((s != '"') && (s != '<'))
    {
        status = false;
    }

    while(status)
    {
        char c = fgetc(m_file);
        if(is_path(c))
        {
            buffer.push_back(c);
        }
        else
        {
            if(((s == '"') && (c == '"')) || ((s == '<') && (c == '>')))
            {
                break;
            }
            else
            {
                status = false;
            }
        }
    }
    return status;
}

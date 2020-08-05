#include "processor.hpp"

#include <algorithm>

#include "ascii.hpp"

enum { INITIAL_STRING_BUFFER_SIZE = 1024 };

Processor::Processor()
{
    m_path_buffer.reserve(INITIAL_STRING_BUFFER_SIZE);
    m_string_buffer.reserve(INITIAL_STRING_BUFFER_SIZE);
}

Processor::~Processor()
{
}

std::pair<bool, uint64_t> Processor::process_include(const char* path)
{
    bool status = true;
    uint64_t write_time = 0;

    OS::FileInfo f_info = {};
    if((status = OS::GetFileInfo(path, f_info)))
    {
        std::map<uint64_t, OS::FileInfo>::const_iterator it = m_cache.find(f_info.ID);
        if(it == m_cache.end())
        {
            std::pair<bool, uint64_t> rval = process_file(path);
            if(rval.first)
            {
                write_time = std::max(f_info.write_time, rval.second);
                
                f_info.write_time = write_time;
                m_cache[f_info.ID] = f_info;
            }
            else
            {
                status = false;
                printf("error: could not process file \"%s\"\n", path);
            }
        }
        else
        {
            write_time = it->second.write_time;
        }
    }

    return std::make_pair(status, write_time);
}

bool Processor::read_token(File& file)
{
    m_string_buffer.clear();
    while(true)
    {
        char c = file.getc();
        if(is_text(c))
        {
            m_string_buffer.push_back(c);
        }
        else
        {
            if(c != EOF)
            {
                file.ungetc(c);
            }
            break;
        }
    }
    return true;
}

bool Processor::read_include(File& file)
{
    file.skip_space();
    m_string_buffer.clear();

    bool status = true;
    char s = file.getc();
    if((s != '"') && (s != '<'))
    {
        status = false;
    }

    while(status)
    {
        char c = file.getc();
        if(is_path(c))
        {
            m_string_buffer.push_back(c);
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

std::pair<bool, uint64_t> Processor::process_file(const char* path)
{
    bool status = true;
    uint64_t write_time = 0;
    
    File file(path);
    if(!file.is_open())
    {
        printf("could not open file \"%s\" for reading\n", path);
        status = false;
    }

    while(status)
    {
        file.skip_space();
        if(file.is_eof())
        {
            break;
        }
        else if(file.getc() == '#')
        {
            read_token(file);
            const char* str = m_string_buffer.data();
            if(strcmp(str, "include") == 0)
            {
                if((status = read_include(file)))
                {
                    m_path_buffer[0] = 0;
                    strncat(m_path_buffer, m_string_buffer.data(), m_path_buffer.size());

                    std::pair<bool, uint64_t> rval = process_include(m_path_buffer);
                    if(rval.first)
                    {
                        write_time = std::max(write_time, rval.second);
                    }
                }
                else
                {
                    printf("bad include\n");
                    break;
                }
            }
            else
            {
                file.skip_line();
            }
        }
        else
        {
            file.skip_line();
        }
    }

    return std::make_pair(status, write_time);
}

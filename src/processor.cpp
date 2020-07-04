#include "processor.hpp"

#include <algorithm>

#include "file.hpp"

Processor::Processor()
{
    m_buffer.reserve(MAX_PATH_LEN);
    m_path_buffer = new char[MAX_PATH_LEN];
}

Processor::~Processor()
{
    delete[] m_path_buffer;
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
            file.read_token(m_buffer);
            const char* str = m_buffer.data();
            if(strcmp(str, "include") == 0)
            {
                if((status = file.read_include(m_buffer)))
                {
                    if(m_buffer.size() >= MAX_PATH_LEN)
                    {
                        status = false;
                        printf("error: include path exceeds size limit\n");
                    }
                    else
                    {
                        m_path_buffer[0] = 0;
                        strncat(m_path_buffer, m_string_buffer.size(), m_path_buffer.size());

                        std::pair<bool, uint64_t> rval = process_include(m_path_buffer);
                        if(rval.first)
                        {
                            write_time = std::max(write_time, rval.second);
                        }
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

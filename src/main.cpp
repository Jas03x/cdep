
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#define NOMINMAX
#include "Windows.h"
#include <fileapi.h>

inline bool is_space(char c) { return (c == ' ') || (c == '\t'); }
inline bool is_text (char c) { return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_'); }
inline bool is_path (char c) { return is_text(c) || (c == ' ') || (c == '/'); }

class _Args
{
private:
    enum TYPE : unsigned int
    {
        INVALID = 0,
        NONE = 1,
        INCLUDE = 2
    };

    std::vector<std::string> m_inc_list;
    std::vector<std::string> m_src_list;

private:
    TYPE parse_arg(const char* arg, const char** val)
    {
        TYPE type = TYPE::INVALID;
        if(arg[0] == '-')
        {
            switch(arg[1])
            {
                case 'I':
                {
                    type = TYPE::INCLUDE;
                    *val = (arg[2] == 0) ? nullptr : &arg[2];
                    break;
                }
                default:
                {
                    type = TYPE::INVALID;
                    printf("invalid switch '%c'\n", arg[1]);
                    break;
                }
            }
        }
        else
        {
            type = TYPE::NONE;
        }
        return type;
    }

    bool path_valid(const char* path)
    {
        bool status = true;
        for(unsigned int i = 0 ;; i++)
        {
            char c = path[i];
            if(!is_path(c))
            {
                if(c == 0) {
                    break;
                } else {
                    status = false;
                    printf("bad include path\n");
                }
            }
        }
        return status;
    }

    bool parse_inc_path(const char* path)
    {
        bool status = path_valid(path);
        if(status)
        {
            m_inc_list.push_back(std::string(path));
        }
        return status;
    }

    bool parse_src_path(const char* path)
    {
        bool status = path_valid(path);
        if(status)
        {
            m_src_list.push_back(std::string(path));
        }
        return status;
    }

public:
    bool Parse(int argc, const char** argv)
    {
        bool status = true;
        for(unsigned int i = 1; status && (i < argc); i++)
        {
            const char* arg = argv[i];
            const char* val = nullptr;
            TYPE type = parse_arg(argv[i], &val);
            if(type == TYPE::INVALID)
            {
                status = false;
            }
            else
            {
                switch(type)
                {
                    case TYPE::INCLUDE:
                    {
                        if(val == nullptr) {
                            val = argv[++i];
                        }
                        status = parse_inc_path(val);
                        break;
                    }
                    case TYPE::NONE:
                    {
                        status = parse_src_path(arg);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }
        return status;
    }

    const std::vector<std::string>& GetIncList() const { return m_inc_list; }
    const std::vector<std::string>& GetSrcList() const { return m_src_list; }
} Args;

class File
{
private:
    FILE* m_file;

public:
    File(const char* path) {
        m_file = fopen(path, "r");
    }

    ~File() {
        if(m_file != nullptr) {
            fclose(m_file);
            m_file = nullptr;
        }
    }
    
    bool is_open()    { return m_file != nullptr; }
    char getc()       { return fgetc(m_file); }
    
    void skip_line()
    {
        while(true) {
            char c = fgetc(m_file);
            if((c == 0) || (c == '\n')) {
                break;
            }
        }
    }
    
    void skip_space()
    {
        while(true) {
            char c = fgetc(m_file);
            if(!is_space(c)) {
                if(c != EOF) {
                    ungetc(c, m_file);
                }
                break;
            }
        }
    }
    
    void read_token(std::vector<char>& buffer)
    {
        buffer.clear();
        while(true) {
            char c = fgetc(m_file);
            if(is_text(c)) {
                buffer.push_back(c);
            } else {
                if(c != EOF) {
                    ungetc(c, m_file);
                }
                break;
            }
        }
    }

    bool read_include(std::vector<char>& buffer)
    {
        skip_space();
        buffer.clear();

        bool status = true;
        char s = fgetc(m_file);
        if((s != '"') && (s != '<')) { status = false; }
        while(status)
        {
            char c = fgetc(m_file);
            if(is_path(c)) {
                buffer.push_back(c);
            } else {
                if(((s == '"') && (c == '"')) || ((s == '<') && (c == '>'))) {
                    break;
                } else {
                    status = false;
                }
            }
        }
        return status;
    }
};

uint64_t make_uint64(uint32_t low, uint32_t high)
{
    uint64_t val = 0;
    uint32_t* ptr = reinterpret_cast<uint32_t*>(&val);
    ptr[1] = low;
    ptr[0] = high;
    return val;
}

struct FileInfo
{
    uint64_t ID;
    uint64_t create_time;
    uint64_t access_time;
    uint64_t write_time;
    bool exists;
};

bool GetFileHandle(const char* path, HANDLE* handle)
{
    bool status = true;
    *handle = CreateFileA(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if(*handle == INVALID_HANDLE_VALUE) {
        status = false;
    }
    return status;
}

bool GetFileInfo(const char* path, FileInfo& info)
{
    HANDLE handle;
    bool status = GetFileHandle(path, &handle);

    BY_HANDLE_FILE_INFORMATION f_info = {};
    if(status)
    {
        info.exists = true;
        if(GetFileInformationByHandle(handle, &f_info) == 0) {
            status = false;
            printf("error: could not scan file \"%s\"\n", path);
        } else {
            info.ID = make_uint64(f_info.nFileIndexLow, f_info.nFileIndexHigh);
            info.create_time = make_uint64(f_info.ftCreationTime.dwLowDateTime,   f_info.ftCreationTime.dwHighDateTime);
            info.access_time = make_uint64(f_info.ftLastAccessTime.dwLowDateTime, f_info.ftLastAccessTime.dwHighDateTime);
            info.write_time  = make_uint64(f_info.ftLastWriteTime.dwLowDateTime,  f_info.ftLastWriteTime.dwHighDateTime);
        }
        CloseHandle(handle);
    } else {
        info.exists = false;
    }

    return status;
}

bool UpdateFileInfo(const char* path, const FileInfo& params)
{
    HANDLE handle;
    bool status = GetFileHandle(path, &handle);
    
    if(!status) {
        status = false;
        printf("could not open file \"%s\" to update timestamp\n", path);
    }

    if(status)
    {
        FILE_BASIC_INFO info;
        info.CreationTime.QuadPart = params.create_time;
        info.LastAccessTime.QuadPart = params.access_time;
        info.LastWriteTime.QuadPart = params.write_time;
        info.ChangeTime.QuadPart = params.write_time;
        info.FileAttributes = 0;

        if(SetFileInformationByHandle(handle, FileBasicInfo, &info, sizeof(FILE_BASIC_INFO)) == 0) {
            status = false;
            printf("error: failed to update information for file \"%s\"\n", path);
        }
    }

    return status;
}

class _Processor
{
public: std::pair<bool, uint64_t> ProcessFile(const char* path);
private:
    std::vector<char> m_buffer;
    std::map<uint64_t, FileInfo> m_fcache;
    uint64_t biggest_time_stamp;

    std::pair<bool, uint64_t> ProcessHeader(const char* path)
    {
        bool status = true;
        uint64_t write_time = 0;

        FileInfo f_info = {};
        if((status = GetFileInfo(path, f_info)))
        {
            std::map<uint64_t, FileInfo>::const_iterator it = m_fcache.find(f_info.ID);
            if(it == m_fcache.end())
            {
                std::pair<bool, uint64_t> rval = ProcessFile(path);
                if(rval.first) {
                    f_info.write_time = std::max(f_info.write_time, rval.second);
                    m_fcache[f_info.ID] = f_info;
                } else {
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

public:
    _Processor() { m_buffer.reserve(256); }
    std::pair<bool, uint64_t> ProcessFile(const char* path)
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
            if(file.getc() == '#')
            {
                file.read_token(m_buffer);
                const char* str = m_buffer.data();
                if(strcmp(str, "include") == 0)
                {
                    if((status = file.read_include(m_buffer)))
                    {
                        std::pair<bool, uint64_t> rval = ProcessHeader(path);
                        if(rval.first) {
                            write_time = std::max(write_time, rval.second);
                        }
                    }
                    else
                    {
                        printf("bad include file name\n");
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
} Processor;

bool ProcessSource(const char* path)
{
    FileInfo info = {};
    bool status = GetFileInfo(path, info);

    if(!status) {
        printf("error: could not process source file \"%s\"\n", path);
    }

    if(status)
    {
        std::pair<bool, uint64_t> rval = Processor.ProcessFile(path);
        if(rval.first) {
            if(rval.second > info.write_time) {
                info.write_time = rval.second;
                UpdateFileInfo(path, info);
            }
        } else {
            status = false;
        }
    }

    return status;
}

int main(int argc, const char** argv)
{
    Args.Parse(argc, argv);

#if 0
    const std::vector<std::string>& inc_lst = Args.GetIncList();
    for(int i = 0; i < inc_lst.size(); i++)
    {
        printf("Inc: %s\n", inc_lst[i].c_str());
    }

    const std::vector<std::string>& src_lst = Args.GetSrcList();
    for(int i = 0; i < src_lst.size(); i++)
    {
        printf("Src: %s\n", src_lst[i].c_str());
    }
#endif

    bool status = true;

    const std::vector<std::string>& src_list = Args.GetSrcList();
    for(int i = 0; status && (i < src_list.size()); i++)
    {
        status = ProcessSource(src_list[i].c_str());
    }

    return status ? 0 : -1;
}

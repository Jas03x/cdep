#include "os.hpp"

#include <stdio.h>

#define NOMINMAX
#include "Windows.h"
#include <fileapi.h>

uint64_t make_uint64(uint32_t low, uint32_t high)
{
    uint64_t val = 0;
    uint32_t* ptr = reinterpret_cast<uint32_t*>(&val);
    ptr[1] = low;
    ptr[0] = high;
    return val;
}

bool GetFileHandle(const char* path, HANDLE* handle)
{
    bool status = true;
    *handle = CreateFileA(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if(*handle == INVALID_HANDLE_VALUE)
    {
        status = false;
    }
    return status;
}

bool OS::GetFileInfo(const char* path, FileInfo& info)
{
    HANDLE handle;
    bool status = GetFileHandle(path, &handle);

    BY_HANDLE_FILE_INFORMATION f_info = {};
    if(status)
    {
        info.exists = true;
        if(GetFileInformationByHandle(handle, &f_info) == 0)
        {
            status = false;
            printf("error: could not scan file \"%s\"\n", path);
        }
        else
        {
            info.ID = make_uint64(f_info.nFileIndexLow, f_info.nFileIndexHigh);
            info.create_time = make_uint64(f_info.ftCreationTime.dwLowDateTime,   f_info.ftCreationTime.dwHighDateTime);
            info.access_time = make_uint64(f_info.ftLastAccessTime.dwLowDateTime, f_info.ftLastAccessTime.dwHighDateTime);
            info.write_time  = make_uint64(f_info.ftLastWriteTime.dwLowDateTime,  f_info.ftLastWriteTime.dwHighDateTime);
        }
        CloseHandle(handle);
    }
    else
    {
        info.exists = false;
    }

    return status;
}

bool OS::UpdateFileInfo(const char* path, const FileInfo& params)
{
    HANDLE handle;
    bool status = GetFileHandle(path, &handle);
    
    if(!status)
    {
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

        if(SetFileInformationByHandle(handle, FileBasicInfo, &info, sizeof(FILE_BASIC_INFO)) == 0)
        {
            status = false;
            printf("error: failed to update information for file \"%s\"\n", path);
        }
    }

    return status;
}

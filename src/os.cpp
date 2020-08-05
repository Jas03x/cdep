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
    bool status = true;
    HANDLE handle = nullptr;
    BY_HANDLE_FILE_INFORMATION data = {};

    if(status)
    {
        status = GetFileHandle(path, &handle);
    }

    if(status)
    {
        status = (GetFileInformationByHandle(handle, &data) == TRUE);
    }

    if(status)
    {
        info.exists = true;
        info.ID = make_uint64(data.nFileIndexLow, data.nFileIndexHigh);
        info.create_time = make_uint64(data.ftCreationTime.dwLowDateTime,   data.ftCreationTime.dwHighDateTime);
        info.access_time = make_uint64(data.ftLastAccessTime.dwLowDateTime, data.ftLastAccessTime.dwHighDateTime);
        info.write_time  = make_uint64(data.ftLastWriteTime.dwLowDateTime,  data.ftLastWriteTime.dwHighDateTime);
    }

    if(handle != nullptr)
    {
        CloseHandle(handle);
    }

    return status;
}

bool OS::SetFileInfo(const char* path, const FileInfo& info)
{
    HANDLE handle;
    bool status = GetFileHandle(path, &handle);
    
    if(!status)
    {
        printf("could not open file \"%s\" to update timestamp\n", path);
    }

    if(status)
    {
        FILE_BASIC_INFO file_info = {};
        file_info.CreationTime.QuadPart = info.create_time;
        file_info.LastAccessTime.QuadPart = info.access_time;
        file_info.LastWriteTime.QuadPart = info.write_time;
        file_info.ChangeTime.QuadPart = info.write_time;
        file_info.FileAttributes = 0;

        if(SetFileInformationByHandle(handle, FileBasicInfo, &file_info, sizeof(FILE_BASIC_INFO)) == 0)
        {
            status = false;
            printf("error: failed to update information for file \"%s\"\n", path);
        }
    }

    if(handle != nullptr)
    {
        CloseHandle(handle);
    }

    return status;
}



#include "file.hpp"

File::File()
{
    handle = nullptr;
}

File::File(const char* path, uint32_t mode)
{
    const char* modes[] = { "r", "w" };

    if(mode < File::MAX)
    {
        handle = fopen(path, modes[mode]);
    }
    else
    {
        printf("error: invalid file mode\n");
    }
}

File::~File()
{
    if(handle != nullptr)
    {
        fclose(handle);
    }
}

File& File::operator=(File& other)
{
    this->handle = other.handle;
    other.handle = nullptr;
    return *this;
}

bool File::eof()
{
    return feof(handle) != 0;
}

bool File::is_open()
{
    return (handle != nullptr);
}

int File::getc()
{
    return fgetc(handle);
}

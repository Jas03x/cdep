#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <map>
#include <utility>
#include <vector>

#include "os.hpp"

class Processor
{
private:
    char* m_path_buffer;
    std::vector<char> m_string_buffer;
    std::map<uint64_t, OS::FileInfo> m_cache;

private:
    std::pair<bool, uint64_t> process_include(const char* path);

public:
    enum { MAX_PATH_LEN = 2048 };

public:
    Processor();
    ~Processor();
    
    std::pair<bool, uint64_t> process_file(const char* path);
};

#endif // PROCESSOR_HPP

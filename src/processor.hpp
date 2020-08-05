#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <map>
#include <utility>
#include <vector>

#include "os.hpp"
#include "file.hpp"

class Processor
{
private:
    std::vector<char> m_path_buffer;
    std::vector<char> m_string_buffer;
    std::map<uint64_t, OS::FileInfo> m_cache;

private:
    bool read_token(File& file);
    bool read_include(File& file);

    std::pair<bool, uint64_t> process_include(const char* path);

public:
    Processor();
    ~Processor();
    
    std::pair<bool, uint64_t> process_file(const char* path);
};

#endif // PROCESSOR_HPP

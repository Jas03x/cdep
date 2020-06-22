#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <map>
#include <utility>
#include <vector>

#include "os.hpp"

class Processor
{
private:
    std::vector<char> m_buffer;
    std::map<uint64_t, OS::FileInfo> m_fcache;

private:
    std::pair<bool, uint64_t> process_include(const char* path);

public:
    Processor() { m_buffer.reserve(256); }
    
    std::pair<bool, uint64_t> process_file(const char* path);
};

#endif // PROCESSOR_HPP
#ifndef ARGS_HPP
#define ARGS_HPP

#include <string>
#include <vector>

class Args
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
    TYPE parse_arg(const char* arg, const char** val);
    bool path_valid(const char* path);
    bool parse_inc_path(const char* path);
    bool parse_src_path(const char* path);

public:
    bool parse(int argc, const char** argv);
    const std::vector<std::string>& get_inc_list() const;
    const std::vector<std::string>& get_src_list() const;
};

#endif // ARGS_HPP
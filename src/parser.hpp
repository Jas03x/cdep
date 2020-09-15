#ifndef PARSER_HPP
#define PARSER_HPP

#include "args.hpp"
#include "file.hpp"

class Parser
{
private:
    File m_File;
    const Args* m_Args;

private:
    Parser(const char* path, const Args& args);
    ~Parser();

public:
    static bool Process(const char* path, const Args& args);
};

#endif // PARSER_HPP
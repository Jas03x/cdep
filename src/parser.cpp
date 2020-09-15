#include "parser.hpp"

Parser::Parser(const char* path, const Args& args)
{
    m_File = File(path, File::READ);
    m_Args = &args;
}

Parser::~Parser()
{
}

bool Parser::Process(const char* path, const Args& args)
{
    Parser parser(path, args);

    
}

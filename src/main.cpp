
#include <cstdio>

#include "args.hpp"
#include "ascii.hpp"
#include "parser.hpp"

bool parse_path(const char* path)
{
    bool status = true;
    for(const char* it = path; *it != 0; it++)
    {
        if(!is_path(*it))
        {
            status = false;
            break;
        }
    }
    return true;
}

bool parse_args(int argc, char** argv, Args& args)
{
    enum : uint32_t
    {
        scan_error  = 0,
        scan_option = 1,
        scan_inc    = 2
    } state = scan_option;

    for(unsigned int i = 0; (i < argc) && (state != scan_error); i++)
    {
        switch(state)
        {
            case scan_option:
            {
                if((argv[i][0] == '-') && (argv[i][2] == '\0'))
                {
                    if(argv[i][1] == 'I')
                    {
                        state = scan_inc;
                    }
                    else
                    {
                        printf("error: unknown switch '%c'\n", argv[i][1]);
                    }
                }
                else
                {
                    state = parse_path(argv[i]) ? scan_option : scan_error;
                    if(state != scan_error)
                    {
                        args.src_list.push_back(std::string(argv[i]));
                    }
                }
                break;
            }
            
            case scan_inc:
            {
                state = parse_path(argv[i]) ? scan_option : scan_error;
                if(state != scan_error)
                {
                    args.inc_list.push_back(std::string(argv[i]));
                }
                break;
            }
        }
    }

    return state;
}

int main(int argc, char** argv)
{
    Args args;
    bool status = parse_args(argc, argv, args);

    for(unsigned int i = 0; status && (i < args.src_list.size()); i++)
    {
        status = Parser::Process(args.src_list[i].c_str(), args);
    }

    return status ? 0 : -1;
}

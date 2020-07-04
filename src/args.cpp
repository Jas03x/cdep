#include "args.hpp"

#include <ascii.hpp>

Args* Instance = nullptr;

Args::Args()
{
    if(Instance == nullptr) {
        Instance = this;
    } else {
        printf("error: args structure already exists\n");
    }
}

Args::~Args()
{
    Instance = nullptr;
}

Args* Args::GetInstance()
{
    if(Instance == nullptr)
    {
        printf("error: args structure not allocated\n");
    }
    return Instance;
}

bool Args::parse(int argc, const char** argv)
{
    bool status = true;
    for(unsigned int i = 1; status && (i < argc); i++)
    {
        const char* arg = argv[i];
        const char* val = nullptr;
        TYPE type = parse_arg(argv[i], &val);
        if(type == TYPE::INVALID)
        {
            status = false;
        }
        else
        {
            switch(type)
            {
                case TYPE::INCLUDE:
                {
                    if(val == nullptr)
                    {
                        val = argv[++i];
                    }
                    status = parse_inc_path(val);
                    break;
                }
                case TYPE::NONE:
                {
                    status = parse_src_path(arg);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }
    return status;
}

Args::TYPE Args::parse_arg(const char* arg, const char** val)
{
    TYPE type = TYPE::INVALID;
    if(arg[0] == '-')
    {
        switch(arg[1])
        {
            case 'I':
            {
                type = TYPE::INCLUDE;
                *val = (arg[2] == 0) ? nullptr : &arg[2];
                break;
            }
            default:
            {
                type = TYPE::INVALID;
                printf("invalid switch '%c'\n", arg[1]);
                break;
            }
        }
    }
    else
    {
        type = TYPE::NONE;
    }
    return type;
}

bool Args::path_valid(const char* path)
{
    bool status = true;
    for(unsigned int i = 0 ;; i++)
    {
        char c = path[i];
        if(!is_path(c))
        {
            if(c == 0)
            {
                break;
            }
            else
            {
                status = false;
                printf("invalid path\n");
            }
        }
    }
    return status;
}

bool Args::parse_inc_path(const char* path)
{
    bool status = path_valid(path);
    if(status)
    {
        m_inc_list.push_back(std::string(path));
    }
    return status;
}

bool Args::parse_src_path(const char* path)
{
    bool status = path_valid(path);
    if(status)
    {
        m_src_list.push_back(std::string(path));
    }
    return status;
}

const std::vector<std::string>& Args::get_inc_list() const { return m_inc_list; }
const std::vector<std::string>& Args::get_src_list() const { return m_src_list; }


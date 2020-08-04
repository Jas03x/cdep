#include "args.hpp"

#include "ascii.hpp"

static Args* Instance = nullptr;

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
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'I':
                {
                    status = parse_inc_path(argv[i][2] != 0 ? &argv[i][2] : argv[++i]);
                    break;
                }
                default:
                {
                    status = false;
                    printf("invalid switch\n");
                    break;
                }
            }
        }
        else
        {
            status = parse_src_path(argv[i]);
        }
    }
    return status;
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


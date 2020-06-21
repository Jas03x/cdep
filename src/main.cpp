
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <string>
#include <vector>

inline bool is_ascii(char c)
{
    return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_') || (c == ' ') || (c == '/') || (c == ':');
}

class _Args
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
    TYPE parse_arg(const char* arg, const char** val)
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

    bool path_valid(const char* path)
    {
        bool status = true;
        for(unsigned int i = 0 ;; i++)
        {
            char c = path[i];
            if(!is_ascii(c))
            {
                if(c == 0) {
                    break;
                } else {
                    status = false;
                    printf("bad include path\n");
                }
            }
        }
        return status;
    }

    bool parse_inc_path(const char* path)
    {
        bool status = path_valid(path);
        if(status)
        {
            m_inc_list.push_back(std::string(path));
        }
        return status;
    }

    bool parse_src_path(const char* path)
    {
        bool status = path_valid(path);
        if(status)
        {
            m_src_list.push_back(std::string(path));
        }
        return status;
    }

public:
    bool Parse(int argc, const char** argv)
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
                        if(val == nullptr) {
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

    const std::vector<std::string>& GetIncList() const { return m_inc_list; }
    const std::vector<std::string>& GetSrcList() const { return m_src_list; }
} Args;

int main(int argc, const char** argv)
{
    Args.Parse(argc, argv);

    const std::vector<std::string>& inc_lst = Args.GetIncList();
    for(int i = 0; i < inc_lst.size(); i++)
    {
        printf("Inc: %s\n", inc_lst[i].c_str());
    }

    const std::vector<std::string>& src_lst = Args.GetSrcList();
    for(int i = 0; i < src_lst.size(); i++)
    {
        printf("Src: %s\n", src_lst[i].c_str());
    }

    return 0;
}

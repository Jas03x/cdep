
#include <stdio.h>

#include "args.hpp"
#include "processor.hpp"

int main(int argc, const char** argv)
{
    bool status = true;
    Args args;
    Processor processor;

    status = args.parse(argc, argv);

    const std::vector<std::string>& src_list = args.get_src_list();
    for(int i = 0; status && (i < src_list.size()); i++)
    {
        const char* path = src_list[i].c_str();

        OS::FileInfo info = {};
        status = OS::GetFileInfo(path, info);

        if(!status)
        {
            printf("error: could not process source file \"%s\"\n", path);
        }

        if(status)
        {
            std::pair<bool, uint64_t> rval = processor.process_file(path);
            if(rval.first)
            {
                if(rval.second > info.write_time)
                {
                    info.write_time = rval.second;
                    OS::UpdateFileInfo(path, info);
                }
            }
            else
            {
                status = false;
            }
        }
    }

    return status ? 0 : -1;
}

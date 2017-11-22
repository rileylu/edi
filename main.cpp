#include <istream>
#include <fstream>
#include "stsocketbuf.hpp"
#include "stsocket.hpp"
#include "ftpclient.h"


void *work(void*)
{
//    FTPClient cli("172.16.120.128","21","lmz","gklmz2013");
    std::vector<std::string> fl;
    try
    {
        FTPClient cli("124.207.27.34","21","gzftpqas01","001testgz");
        cli.open();
        cli.login();
        cli.change_dir("/OUT/stockout/");
        std::istream& is(cli.begin_list("."));
        std::string fn;
        std::size_t pos;
        while(getline(is,fn))
        {
            if(fn.find(".xml")!=fn.npos)
            {
                if ((pos = fn.find('\r')) != fn.npos)
                    fn.erase(pos, 1);
                fl.push_back(std::move(fn));
            }
        }
        cli.end_list();
        cli.logout();
    }
    catch (...)
    {}
    return nullptr;
}
int main()
{
    st_init();
    std::vector<st_thread_t> tds;
    for(int i=0;i<100;++i)
        tds.push_back(st_thread_create(work, nullptr,1,0));
    for(auto td:tds)
        st_thread_join(td, nullptr);
    return 0;
}

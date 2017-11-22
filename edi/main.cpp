#include <istream>
#include "ftpclient.h"
#include <string>
#include <vector>
#include "filesession.hpp"


void *work(void*)
{
//    FTPClient cli("172.16.120.128","21","lmz","gklmz2013");
    bool working=false;
    do
    {
        try
        {
            FTPClient cli("124.207.27.34","21","gzftpqas01","001testgz");
            cli.open();
            cli.login();
            cli.change_dir("/OUT/stockout/");
            std::istream& is(cli.begin_list("."));
            std::string fn;
            std::size_t pos;
            FileStream f;
            fn="list";
            fn+=std::to_string(reinterpret_cast<long>(st_thread_self()));
            fn+=".txt";
            FileSession fs(fn,O_CREAT|O_WRONLY|O_TRUNC);
            while(getline(is,fn))
            {
                if(fn.find(".xml")!=fn.npos)
                {
                    if ((pos = fn.find('\r')) != fn.npos)
                        fn.erase(pos, 1);
                    fs.io()<<fn<<'\n';
                }
            }
            fs.io()<<std::flush;
            cli.end_list();
            cli.logout();
            working=false;
        }
        catch (...)
        {
            perror("error");
            working=true;
        }
    }while(working);
    return nullptr;
}
int main()
{
    st_set_eventsys(ST_EVENTSYS_ALT);
    st_init();
    std::vector<st_thread_t> tds;
    for(int i=0;i<100;++i)
        tds.push_back(st_thread_create(work, nullptr,1,0));
    for(auto td:tds)
        st_thread_join(td, nullptr);
    return 0;
}

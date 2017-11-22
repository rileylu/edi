#include <istream>
#include "ftpclient.h"
#include <string>
#include <vector>
#include "filestream.hpp"


void *work(void*)
{
//    FTPClient cli("172.16.120.128","21","lmz","gklmz2013");
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
        f.open(fn.c_str(), O_WRONLY|O_CREAT|O_TRUNC,0755);
        while(getline(is,fn))
        {
            if(fn.find(".xml")!=fn.npos)
            {
                if ((pos = fn.find('\r')) != fn.npos)
                    fn.erase(pos, 1);
                STStreamBuf fbuf(f);
                BufferedIOStream ss(nullptr,&fbuf);
                ss<<fn<<'\n';
            }
        }
        cli.end_list();
        cli.logout();
    }
    catch (...)
    {
        perror("error");
    }
    return nullptr;
}
int main()
{
    st_set_eventsys(ST_EVENTSYS_ALT);
    st_init();
    std::vector<st_thread_t> tds;
    for(int i=0;i<50;++i)
        tds.push_back(st_thread_create(work, nullptr,1,0));
    for(auto td:tds)
        st_thread_join(td, nullptr);
    return 0;
}

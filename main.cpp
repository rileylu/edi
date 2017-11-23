#include <istream>
#include "ftpclient.h"
#include <string>
#include <vector>
#include "filesession.hpp"
#include <sys/stat.h>


void *work(void*)
{
    //    FTPClient cli("172.16.120.128","21","lmz","gklmz2013");
    bool retry=false;
    do
    {
        try
        {
            std::string dir=std::to_string(reinterpret_cast<long>(st_thread_self()));
            mkdir(dir.c_str(),0755);
            FileSession fs("list.txt",O_RDONLY);
            std::string fn;
            FTPClient cli("124.207.27.34","21","gzftpqas01","001testgz");
            cli.open();
            cli.login();
            cli.change_dir("/OUT/stockout/");
            while(std::getline(fs.io(),fn))
            {
                std::istream& is(cli.begin_download(fn));
                FileSession newFile(dir+"/"+fn,O_CREAT|O_WRONLY|O_TRUNC);
                newFile.io()<<is.rdbuf();
                cli.end_download();
            }
            cli.logout();
            retry=false;
        }
        catch (...)
        {
            perror("error");
            retry=true;
            st_sleep(10);
        }
    }while(retry);
    
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

#include <istream>
#include "stsocketbuf.hpp"
#include "stsocket.hpp"
#include "ftpclient.h"

void *work(void*)
{
    FTPClient cli("172.16.120.128","21","lmz","gklmz2013");
    cli.open();
    cli.login();
    cli.change_dir("tmp");
    std::istream& is(cli.begin_list("."));
    std::string fn;
    while(std::getline(is,fn))
        ::fprintf(stdout,"%s\n",fn.c_str());
    cli.end_list();
    cli.logout();
    return nullptr;
}
int main()
{
    st_init();
    std::vector<st_thread_t> tds;
    for(int i=0;i<1;++i)
        tds.push_back(st_thread_create(work, nullptr,1,0));
    for(auto td:tds)
        st_thread_join(td, nullptr);
    return 0;
}

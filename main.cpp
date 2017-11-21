#include <istream>
#include "stsocketbuf.hpp"
#include "stsocket.hpp"
#include "utilities.hpp"

#define TIMEOUT 1000000000
void *work(void*)
{
    STSocket sock(TIMEOUT);
    addrinfo servaddr=edi::getaddrinfo("172.16.120.128", "21");
    try
    {
        sock.connect(servaddr.ai_addr, servaddr.ai_addrlen);
        STSocketBuf buf(sock);
        std::ostream os(&buf);
        std::string cmd{"USER lmz\r\nPASS gklmz2013\r\n"};
        os.write(cmd.c_str(), cmd.size());
        os.flush();
        STSocketBuf inBuf(sock);
        std::string line;
        std::istream is(&inBuf);
        while(std::getline(is,line))
            fprintf(stdout,"%s\n",line.c_str());
    }
    catch (...)
    {
        perror("error");
    }
    return nullptr;
}
int main()
{
    st_init();
    std::vector<st_thread_t> tds;
    for(int i=0;i<10;++i)
        tds.push_back(st_thread_create(work, nullptr,1,0));
    for(auto td:tds)
        st_thread_join(td, nullptr);
    return 0;
}

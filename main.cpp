#include "stsocketbuf.hpp"
#include "stsocket.hpp"
#include <ostream>
#include <vector>
#include <string>
#include "utilities.hpp"

#define TIMEOUT 1000000000
void *work(void*)
{
    STSocket sock(TIMEOUT);
    addrinfo servaddr=edi::getaddrinfo("172.16.120.128", "21");
    sock.connect(servaddr.ai_addr, sizeof(servaddr.ai_addrlen));
    STSocketBuf buf(sock);
    std::ostream os(&buf);
    std::string cmd{"USER lmz\r\n"};
    os.write(cmd.c_str(), cmd.size());
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

//
//  stsocket.hpp
//  testst
//
//  Created by lmz on 19/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef stsocket_hpp
#define stsocket_hpp
#include <st.h>
class STSocket
{
public:
    STSocket(st_utime_t timeout);
    ~STSocket();
    ssize_t read(void *data,size_t len);
    ssize_t write(const void* data,size_t len);
    int connect(struct sockaddr* addr,int addrlen);
    st_utime_t get_timeout() const;
    STSocket(const STSocket&)=delete;
    STSocket& operator=(const STSocket &)=delete;
private:
    static void sock_destructor(void *args);
    int native_sock_;
    ::st_netfd_t sock_;
    st_utime_t timeout_;
};
inline st_utime_t STSocket::get_timeout() const
{
    return timeout_;
}


#endif /* stsocket_hpp */

//
//  filesession.hpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef filesession_hpp
#define filesession_hpp

#include "basesession.hpp"
#include "filestream.hpp"

class FileSession:public BaseSession
{
public:
    FileSession(const std::string& fn,int flags,mode_t mode=0755,st_utime_t timeout=60*UNIT);
    ~FileSession();
private:
    void open();
private:
    FileStream fs_;
};

#endif /* filesession_hpp */

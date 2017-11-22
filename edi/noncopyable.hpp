//
//  noncopyable.hpp
//  edi
//
//  Created by lmz on 22/11/2017.
//  Copyright © 2017 com.oocl. All rights reserved.
//

#ifndef noncopyable_hpp
#define noncopyable_hpp

class Noncopyable
{
public:
    Noncopyable(const Noncopyable&)=delete;
    Noncopyable& operator=(const Noncopyable&)=delete;
protected:
    Noncopyable()=default;
};

#endif /* noncopyable_hpp */

//
// Created by Lmz on 12/07/2017.
//

#ifndef EDI_NONCOPYABLE_HPP
#define EDI_NONCOPYABLE_HPP

class Noncopyable {
protected:
    Noncopyable() = default;

    ~Noncopyable() = default;

    Noncopyable(const Noncopyable &) = delete;

    Noncopyable &operator=(const Noncopyable &)= delete;
};


#endif //EDI_NONCOPYABLE_HPP

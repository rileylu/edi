#pragma once
#include <netdb.h>
#include <string>
#include <sys/types.h>

namespace edi {
addrinfo getaddrinfo(const std::string& host, const std::string& servname);
}

#pragma once
#include <netdb.h>
#include <string>
#include <sys/types.h>

namespace edi {
const int TIMEOUT_UNIT = 1000000;
extern addrinfo getaddrinfo(const std::string& host, const std::string& servname);
}

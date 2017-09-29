#include "net.h"
#include <iostream>

using edi::Net;

Net* Net::getInstance()
{
	static Net net;
	return &net;
}

Net::~Net()
{
	WSACleanup();
}

Net::Net()
{
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
}
Net* net = Net::getInstance();

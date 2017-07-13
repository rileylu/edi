#pragma once
#include "singleton.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")

struct  Net
{
	Net()
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0)
		{
			std::cerr << "WSAStartup() error" << std::endl;
			exit(1);
		}
	}
private:
	WSADATA wsaData_;
};


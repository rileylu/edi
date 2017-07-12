#pragma once
#include "net.h"
#include <thread>

class IOCP
{
public:
	IOCP()
	{
		iocphd_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, std::thread::hardware_concurrency());
	}
private:
	HANDLE iocphd_;

};
#include "iocp_loop.h"
using edi::IOCP;
using edi::IOCPLoop;
using edi::Context;
Context::Context()
	:_socket(INVALID_SOCKET)
{
}

Context::~Context()
{
}

IOCPLoop::IOCPLoop()
	:_running(false), _completionPort(INVALID_HANDLE_VALUE)
{
}

IOCPLoop::~IOCPLoop()
{
	for (auto &p : _IOThreads)
	{
		if (p.joinable())
			p.join();
	}
	if (_completionPort != INVALID_HANDLE_VALUE)
		::CloseHandle(_completionPort);
}

void IOCPLoop::Start()
{
	_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	for (auto i = 0; i < std::thread::hardware_concurrency() * 2 + 1; ++i)
		_IOThreads.emplace_back(std::bind(&IOCPLoop::_IOThreadFun, this));
	_running.store(true);
}

void IOCPLoop::Stop()
{
}


void IOCPLoop::_IOThreadFun()
{
	DWORD numberOfBytesTransferred = 0;
	LPOVERLAPPED lpOverlapped = nullptr;
	ULONG_PTR key;
	while (_running.load())
	{
		int ret = ::GetQueuedCompletionStatus(_completionPort, &numberOfBytesTransferred, &key, &lpOverlapped, -1);
		IOCP::PerIOData *perIoData = reinterpret_cast<IOCP::PerIOData*>(lpOverlapped);
		Context *context = reinterpret_cast<Context*>(key);
		if (perIoData->operationType == IOCP::OperationType::Read)
		{
			//do something sync??

		}
		else if (perIoData->operationType == IOCP::OperationType::Write)
		{
		}
		IOCP::PerIOData *newData = new IOCP::PerIOData;
		DWORD flag = 0;
		::WSARecv(context->GetSocket(), &newData->wsabuf, 1, NULL, &flag, &newData->overlapped, NULL);
	}
}

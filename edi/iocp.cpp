#include "iocp.h"
#include "countdown.h"
#include <iostream>

IOCP::IOCP()
	:iocphd_(INVALID_HANDLE_VALUE),
	running_(false),
	numOfThreads_(std::thread::hardware_concurrency() * 2)
{
	iocphd_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, std::thread::hardware_concurrency());
	if (!iocphd_)
	{
		//log
		exit(1);
	}
	countDown_.reset(new CountDown(numOfThreads_));
}

IOCP::~IOCP()
{
	for (auto it = workerThreads_.begin(); it != workerThreads_.end(); ++it)
		if (it->joinable())
			it->join();
	::CloseHandle(iocphd_);
}

void IOCP::start()
{
	//log
	std::cout << "IOCP is starting" << std::endl;
	for (auto i = 0; i < numOfThreads_; ++i)
		workerThreads_.emplace_back(&IOCP::workerThread, this);
	countDown_->wait();
	running_ = true;
	std::cout << "IOCP is running" << std::endl;
}

void IOCP::workerThread()
{
	//log
	std::cout << "WorkerThread " << std::this_thread::get_id() << " is running" << std::endl;
	countDown_->decrease();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}

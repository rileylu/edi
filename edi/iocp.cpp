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

void IOCP::stop()
{
}

void IOCP::workerThread()
{
	//log
	static thread_local std::thread::id t_thread_id = std::this_thread::get_id();
	std::cout << "WorkerThread " <<t_thread_id << " is running" << std::endl;
	countDown_->countDown();
	while (true)
	{
		std::cout << "WorkerThread " << t_thread_id << " process callback" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}

#pragma once
#include "net.h"
#include <thread>
#include <vector>
#include <atomic>
#include <memory>

class CountDown;
class IOCP
{
public:
	IOCP();
	~IOCP();
	void start();
	void stop();
	bool isRunning() const
	{
		return running_;
	}
private:
	void workerThread();
	HANDLE iocphd_;
	std::atomic_bool running_;
	size_t numOfThreads_;
	std::vector<std::thread> workerThreads_;
	std::unique_ptr<CountDown> countDown_;
};
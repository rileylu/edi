#pragma once
#include <mutex>
class CountDown
{
public:
	CountDown(int n) :n_(n)
	{}
	void countDown()
	{
			std::lock_guard<std::mutex> lk(m_);
			if((--n_)==0)
				cv_.notify_one();
	}
	void wait()
	{
		std::unique_lock<std::mutex> lk(m_);
		cv_.wait(lk, [this] {
			return n_ == 0;
		});
	}
private:
	int n_;
	std::mutex m_;
	std::condition_variable cv_;
};

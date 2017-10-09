#pragma once
#include <list>
#include <mutex>
#include <condition_variable>

template<typename T>
class SyncQueue
{
public:
	SyncQueue(int maxSize)
		:_maxSize(maxSize), _needStop(false)
	{
	}
	void Put(const T& x)
	{
		Add(x);
	}
	void Put(T&& x)
	{
		Add(std::move(x));
	}
	void Take(std::list<T>& l)
	{
		std::unique_lock<std::mutex> locker(_mutex);
		_notEmpty.wait(locker, [this] {return _needStop || NotEmpty(); });
		if (_needStop)
			return;
		l = std::move(_queue);
		_notFull.notify_one();
	}
	void Take(T& t)
	{
		std::unique_lock<std::mutex> locker(_mutex);
		_notEmpty.wait(locker, [this] {return _needStop || NotEmpty(); });
		if (_needStop)
			return;
		t = _queue.front();
		_queue.pop_front();
		_notFull.notify_one();
	}
	void Stop()
	{
		{
			std::lock_guard<std::mutex> locker(_mutex);
			_needStop = true;
		}
		_notFull.notify_all();
		_notEmpty.notify_all();
	}

	bool Empty()
	{
		std::lock_guard<std::mutex>	locker(_mutex);
		return _queue.empty();
	}

	bool Full()
	{
		std::lock_guard<std::mutex> locker(_mutex);
		return _queue.size() == _maxSize;
	}
	size_t Size()
	{
		std::lock_guard<std::mutex> locker(_mutex);
		return _queue.size();
	}
	int Count()
	{
		return _queue.size();
	}

private:
	bool NotFull() const
	{
		return _queue.size() < _maxSize;
	}
	bool NotEmpty() const
	{
		return !_queue.empty();
	}

	template<typename F>
	void Add(F&& f)
	{
		std::unique_lock<std::mutex> locker(_mutex);
		_notFull.wait(locker, [this] {return _needStop || NotFull(); });
		if (_needStop)
			return;
		_queue.push_back(std::forward<F>(f));
		_notEmpty.notify_one();
	}
private:
	std::list<T> _queue;
	std::mutex _mutex;
	std::condition_variable _notEmpty;
	std::condition_variable _notFull;
	int _maxSize;
	bool _needStop;
};
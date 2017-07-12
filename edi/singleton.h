#pragma once
#include "noncopyable.h"
#include <mutex>
#include <memory>

template<typename T>
class Singleton :Noncopyable
{
public:
	template<typename... Args>
	static T& Instance(Args&&... args)
	{
		std::call_once(once_, [&args...]{
			data_.reset(new T(std::forward<Args>(args)...));
		});
		return *data_;
	}
private:
	Singleton() = default;
	static std::once_flag once_;
	static std::unique_ptr<T> data_;
};

template<typename T>
std::once_flag Singleton<T>::once_;

template<typename T>
std::unique_ptr<T> Singleton<T>::data_ = nullptr;
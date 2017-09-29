#pragma once

template<typename T>
class Singleton
{
public:
	template<typename... Args>
	static T* Instance(Args&&... args)
	{
		if (_instance == nullptr)
			_instance = new T(std::forward<Args>(args)...);
		return _instance;
	}
	Singleton(const Singleton&)=delete;
	Singleton& operator=(const Singleton&)=delete;
protected:
	Singleton() = default;
private:
	static T* _instance;
};

template<typename T>
T* Singleton<T>::_instance = nullptr;
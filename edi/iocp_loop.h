#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include "net.h"
#include "Singleton.h"
#include <map>
#include <memory>

namespace edi {
	struct Per_IO_Data
	{

	};

	class IOCP
	{
	public:
		const static size_t DataBuffSize = 512;
		enum class OperationType
		{
			Read,
			Write
		};

		struct PerIOData
		{
			OVERLAPPED overlapped;
			WSABUF wsabuf;
			char buf[DataBuffSize];
			OperationType operationType;
		};

		using PerIODataPtr = std::shared_ptr<PerIOData>;

	};
	class Context
	{
	public:
		Context();
		~Context();
		SOCKET GetSocket() const
		{
			return _socket;
		}
	private:
		SOCKET _socket;
	};

	class IStream;

	class IOCPLoop
	{
	public:
		IOCPLoop();
		~IOCPLoop();
		void Start();
		void Stop();
	private:
		void _IOThreadFun();
	private:
		std::atomic<bool> _running;
		HANDLE _completionPort;
		std::vector<std::thread> _IOThreads;
	};
}

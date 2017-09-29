#pragma once
#include "ByteArray.h"
#include <functional>

namespace edi {
	class IStream {
	public:
		using DataHandler = std::function<size_t(const char* buf, size_t len)>;
		virtual size_t Receive(char* buffer, size_t buffSize, size_t& readSize) = 0;
		virtual size_t Send(const ByteArray& byteArray) = 0;
		virtual void OnData(DataHandler handler) = 0;
		virtual DataHandler GetDataHandler() = 0;
	};
}

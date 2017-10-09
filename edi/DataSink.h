#pragma once
#include "IStream.h"

namespace edi {
	class DataSink {
	public:
		virtual size_t Write(IStream* stream, const char* buf, size_t bytes) = 0;
		IStream::DataHandler StreamHandler(IStream* stream) {
			return std::bind(&DataSink::Write, this, stream, std::placeholders::_1, std::placeholders::_2);
		}
	};
}

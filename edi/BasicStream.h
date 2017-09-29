#pragma once
#include "IStream.h"
#include "Socket.h"
#include "DataSink.h"
#include "noncopyable.h"

namespace edi {
	class BasicStream :public IStream, public Socket, public NonCopyable {
	public:
		BasicStream() = default;
		BasicStream(SOCKET nativeSocket)
			:Socket(nativeSocket)
		{}

		virtual void SetDataSink(DataSink* dataSink) {
			_dataSink = dataSink;
		}
		virtual DataSink* GetDataSink() {
			return _dataSink;
		}
		virtual const DataSink* GetDataSink() const {
			return _dataSink;
		}
	private:
		DataSink* _dataSink;

	};
}

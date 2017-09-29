#pragma once
#include "BasicStream.h"
#include "iocp_loop.h"

namespace edi {
	class IOCPStream :public BasicStream,public NonCopyable {
	public:
		IOCPStream(SOCKET clientSocket)
			:BasicStream(clientSocket)
		{}
		virtual size_t Receive(char* buffer, size_t buffersize, size_t& readSize) override;
		virtual size_t Send(const ByteArray& byteArray) override;

		void SetPerIOData(IOCP::PerIODataPtr perIOData) {
			_perIOData = perIOData;
		}
		IOCP::PerIODataPtr GetPerIOData() {
			return _perIOData;
		}
		IOCP::PerIODataPtr GetPerIOData() const {
			return _perIOData;
		}
	private:
		IOCP::PerIODataPtr _perIOData;
	};
}
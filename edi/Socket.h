#pragma once
#include "net.h"
#include "noncopyable.h"

namespace edi {

	class Socket :public NonCopyable {
	public:
		Socket() :_nativeSocket(INVALID_SOCKET) {}
		Socket(SOCKET nativeSocket) :_nativeSocket(nativeSocket) {}
		virtual ~Socket()
		{
			::closesocket(_nativeSocket);
		}
		SOCKET	GetNativeSocket() const {
			return _nativeSocket;
		}
		void SetNativeSocket(SOCKET nativeSocket) {
			_nativeSocket = nativeSocket;
		}
	private:
		SOCKET _nativeSocket;
	};

}

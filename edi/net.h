#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib,"ws2_32.lib")

namespace edi {
	class Net
	{
	public:
		static Net* getInstance();
		Net(const Net&) = delete;
		Net& operator=(const Net&) = delete;
		~Net();
	private:
		Net();
	};
}

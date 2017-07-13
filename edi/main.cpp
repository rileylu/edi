#include "iocp.h"
#include "singleton.h"
#include <iostream>

int main()
{
	IOCP &iocp = Singleton<IOCP>::Instance();
	iocp.start();
	return 0;
}
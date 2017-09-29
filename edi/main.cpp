#include "iocp_loop.h"
#include "Singleton.h"
#include "SyncQueue.h"
int main()
{
	SyncQueue<int> q(100);
	q.Put(1);
	return 0;
}
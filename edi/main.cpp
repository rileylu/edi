//#include <boost/predef.h>
//#ifdef BOOST_OS_WINDOWS
//#define _WIN32_WINNT 0x0501
//#if _WIN32_WINNT <= 0x0502
//#define BOOST_ASIO_DISABLE_IOCP
//#define BOOST_ASIO_ENABLE_CANCELIO
//#endif
//#endif
//
#include "ftpcontext.h"
#include <thread>
#include <iostream>
#include <vector>
#include <fstream>
#include "syncqueue.hpp"

using namespace boost;

int main()
{
	asio::io_service ios;
	asio::io_service::work w(ios);
	std::vector<std::thread> tds;
	std::shared_ptr<SyncQueue<std::string>> fileList(new SyncQueue<std::string>());

	std::ifstream in("list.txt");
	std::string line;
	while (std::getline(in, line))
	{
		if (line.find(".xml") != line.npos)
		{
			std::size_t pos;
			if ((pos = line.find('\r')) != line.npos)
				line.erase(pos, 1);
			std::string tmp = std::string("/OUT/stockout/" + line);
			fileList->Putback(std::move(tmp));
		}
	}
	in.close();

	int concurrent = std::thread::hardware_concurrency() * 2;
	for (int i = 0; i < concurrent; ++i)
	{
		tds.emplace_back([&ios] {
			ios.run();
		});
	}

	std::vector<std::shared_ptr<FtpContext>> ftpContexts;
	for (int i = 0; i < 50; ++i)
	{
		ftpContexts.emplace_back(std::make_shared<FtpContext>(ios, "124.207.27.34", 21, "gzftpqas01", "001testgz"));
		//ftpContexts.emplace_back(std::make_shared<FtpContext>(ios, "127.0.0.1", 21, "lmz", "gklmz2013"));
	}

	for (auto& p : ftpContexts)
	{
		p->SendFile(fileList);
	}
	for (auto &p : tds)
	{
		if (p.joinable())
			p.join();
	}
	return 0;
}
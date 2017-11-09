#include "sessionmgr.hpp"

void SessionManagement::Start()
{
	switch (_workType)
	{
	case WorkType::RETR:
		for (auto &ftp : _ftpContextList)
			ftp->RecvFile();
		break;
	case WorkType::STOR:
		for (auto &ftp : _ftpContextList)
			ftp->SendFile();
		break;
	}
}
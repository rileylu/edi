#pragma once
#include "syncqueue.hpp"
#include "ftpcontext.h"
#include <list>
#include <memory>

class SessionManagement
{
public:
	enum class WorkType
	{
		RETR,
		STOR
	};
	SessionManagement(int nworkers, std::shared_ptr<SyncQueue<std::string>> fileList, WorkType workType = WorkType::RETR);
	void Start();
	void Stop();
private:
	WorkType _workType;
	std::list<std::shared_ptr<FtpContext>> _ftpContextList;
	std::shared_ptr<SyncQueue<std::string>> _fileList;
};
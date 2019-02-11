#ifndef LOGFILE_H
#define LOGFILE_H

#include "MutexLock.hpp"
#include "FileUtil.h"
#include "noncopyable.h"
#include <memory>
#include <string>


class LogFile : noncopyable
{
public:
	LogFile(const std::string &basename, int flushEveryN = 1024);
	~LogFile();

	void append(const char *logline, int len);
	void flush();
	bool rollFile();

private:
	void append_unlocked(const char *logline, int len);

	const std::string basename_;
	//flush every N
	const int flushEveryN_;

	int count_;
	std::unique_ptr<MutexLock> mutex_;
	std::unique_ptr<AppendFile> file_;
};

#endif

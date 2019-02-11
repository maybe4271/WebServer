#include "LogFile.h"
#include "FileUtil.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

LogFile::LogFile(const std::string &basename, int flushEveryN) : basename_(basename), flushEveryN_(flushEveryN), count_(0), mutex_(new MutexLock)
{
	file_.reset(new AppendFile(basename_));
}

LogFile::~LogFile() = default;

void LogFile::append(const char *logline, int len)
{
	if (mutex_)
	{
		MutexLockGuard lock(*mutex_);
		append_unlocked(logline, len);
	}
	else
	{
		append_unlocked(logline, len);
	}
}

void LogFile::flush()
{
	if (mutex_)
	{
		MutexLockGuard lock(*mutex_);
		file_->flush();
	}
	else
	{
		file_->flush();
	}
}

void LogFile::append_unlocked(const char *logline, int len)
{
	file_->append(logline, len);
	++count_;
	if (count_ >= flushEveryN_)
	{
		count_ = 0;
		file_->flush();
	}
}

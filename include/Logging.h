#ifndef LOGGING_H
#define LOGGING_H

#include "LogStream.h"
#include <string.h>
#include <string>
#include <stdio.h>

class AsyncLogging;

class Logger
{
public:
	Logger(const char *logline, int len);
	~Logger();
	LogStream &stream() { return impl_.stream_; }

	static void setLogFileStream(std::string fileName)
	{
		logFileName_ = fileName;
	}

	static std::string getLogFileName()
	{
		return logFileName_;
	}
private:
	class Impl
	{
	public:
		Impl(const char *fileName, int line);
		void formatTime();

		LogStream stream_;
		int line_;
		std::string basename_;
	};

	Impl impl_;
	static std::string logFileName_;
};

#define LOG Logger(__FILE__, __LINE__).stream()



#endif

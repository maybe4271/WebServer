#ifndef ASYNCLOGGING_H
#define ASYNCLOGGING_H

#include "noncopyable.h"
#include "MutexLock.hpp"
#include "Thread.h"
#include "LogStream.h"
#include "CountDownLatch.h"
#include "Condition.hpp"

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <memory>

class AsyncLogging : noncopyable
{
public:
	AsyncLogging(const std::string basename, int flushInterval = 2);
	~AsyncLogging()
	{
		std::cout << "~AsyncLogging()" << std::endl;
		if (running_)
			stop();
	}

	void append(const char *logline, int len);

	void start()
	{
		running_ = true;
		thread_.start();
		latch_.wait();
	}

	void stop()
	{
		std::cout << "stop()" << std::endl;
		running_ = false;
		cond_.notify();
		thread_.join();
	}

private:
	void ThreadFunc();
	typedef FixedBuffer<kLargeBuffer> Buffer;
	typedef std::vector<std::shared_ptr<Buffer> > BufferVector;
	typedef std::shared_ptr<Buffer> BufferPtr;
	
	const int flushInterval_;
	bool running_;
	std::string basename_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	BufferVector buffers_;
	CountDownLatch latch_;
};

#endif

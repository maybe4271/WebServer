#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H
#include "../include/noncopyable.h"
#include "../include/Logging.h"
#include "EventLoopThread.h"

#include <memory>
#include <vector>

class EventLoopThreadPool : noncopyable
{
public:
	EventLoopThreadPool(EventLoop *baseLoop, int numThreads);
	~EventLoopThreadPool()
	{
		LOG << "~EventLoopThreadPool()";
	}

	void start();

	EventLoop *getNextLoop();
private:
	EventLoop *baseLoop_;
	bool started_;
	int numThreads_;
	int next_;
	std::vector<std::shared_ptr<EventLoopThread> > threads_;
	std::vector<EventLoop *> loops_;
};


#endif

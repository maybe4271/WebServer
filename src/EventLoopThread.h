#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H
#include "../include/Condition.hpp"
#include "../include/MutexLock.hpp"
#include "../include/Thread.h"
#include "../include/noncopyable.h"
#include "EventLoop.h"

class EventLoopThread : noncopyable
{
public:
	EventLoopThread();
	~EventLoopThread();
	EventLoop *startLoop();
private:
	void threadFunc();
	EventLoop *loop_;
	bool exiting_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
};


#endif

#ifndef SERVER_H
#define SERVER_H
#include "EventLoop.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include <memory>
#include <iostream>

class Server
{
public:
	Server(EventLoop *loop, int threadNum, int port);
	~Server() { }
	EventLoop *getLoop() const { return loop_; }
	void start();
	void handNewConn();
	void handThisConn() { loop_->updatePoller(acceptChannel_); }
private:
	EventLoop *loop_;
	int threadNum_;
	std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
	bool started_;
	std::shared_ptr<Channel> acceptChannel_;
	int port_;
	int listenFd_;
	static const int MAXFDS = 100000;
};



#endif

#include "Channel.h"
#include "Util.h"
#include "EventLoop.h"
#include "Epoll.h"
#include <iostream>
#include <unistd.h>
#include <queue>
#include <cstdlib>


Channel::Channel(EventLoop *loop) : loop_(loop), events_(0), lastEvents_(0)
{
	
}

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd), events_(0), lastEvents_(0)
{
	
}

Channel::~Channel()
{
	
}

int Channel::getFd()
{
	return fd_;
}

void Channel::setFd(int fd)
{
	fd_ = fd;
}

void Channel::handleRead()
{
	if (readHandler_)
	{
		readHandler_();
	}
}

void Channel::handleWrite()
{
	if (writeHandler_)
	{
		writeHandler_();
	}
}

void Channel::handleConn()
{
	if (connHandler_)
	{
		connHandler_();
	}
}


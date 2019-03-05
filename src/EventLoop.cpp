#include "EventLoop.h"
#include "../include/Logging.h"
#include "Util.h"
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <iostream>

__thread EventLoop *t_loopThisThread;

int createEventFd()
{
	int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (efd < 0)
	{
		LOG << "efd create error!";
		abort();
	}
	return efd;
}

EventLoop::EventLoop() : looping_(false), poller_(new Epoll()), wakeupFd_(createEventFd()), quit_(false), eventHandling_(false), callingPendingFunctors_(false), threadId_(CurrentThread::tid()), pwakeupChannel_(new Channel(this, wakeupFd_))
{
	if (t_loopThisThread)
	{
		LOG << "Another EventLoop use this thread!";
		abort();
	}
	else
	{
		t_loopThisThread = this;
	}
	pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
	pwakeupChannel_->setReadHandler(std::bind(&EventLoop::handleRead, this));
	pwakeupChannel_->setConnHandler(std::bind(&EventLoop::handleConn, this));
	//it may need to be modified
	poller_->epoll_add(pwakeupChannel_, 0);
}

void EventLoop::handleConn()
{
	updatePoller(pwakeupChannel_, 0);
}

EventLoop::~EventLoop()
{
	close(wakeupFd_);
	t_loopThisThread = NULL;
}

void EventLoop::wakeup()
{
	//8 bits
	uint64_t one = 1;
	ssize_t n = writen(wakeupFd_, (char *)(&one), sizeof(one));
	if (n != sizeof(one))
	{
		LOG << "EventLoop::wakeup() writes " << n << "bytes instead of 8";
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = readn(wakeupFd_, &one, sizeof(one));
	if (n != sizeof(one))
	{
		LOG << "EventLoop::handleRead() reads " << n << "bytes instead of 8";
	}
	pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::runInLoop(Functor &&cb)
{
	if (isInLoopThread())
		cb();
	else
		queInLoop(std::move(cb));
}

void EventLoop::queInLoop(Functor &&cb)
{
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.emplace_back(std::move(cb));
	}

	if (!isInLoopThread() || callingPendingFunctors_)
		wakeup();
}

void EventLoop::loop()
{
	assert(!looping_);
	assert(isInLoopThread());
	looping_ = true;
	quit_ = false;
	std::vector<SP_Channel> ret;

	while (!quit_)
	{
		ret.clear();
		ret = poller_->poll();
		eventHandling_ = true;
		for (auto &it : ret)
		{
			it->handleEvents();
		}

		eventHandling_ = false;
		//执行回调函数
		doPendingFunctors();
		poller_->handleExpired();
	}
	looping_ = false;
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;

	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (size_t i = 0; i <functors.size(); i++)
	{
		functors[i]();
	}
	callingPendingFunctors_ = false;
}

void EventLoop::quit()
{
	quit_ = true;
	if (!isInLoopThread())
	{
		wakeup();
	}
}

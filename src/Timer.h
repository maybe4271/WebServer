#ifndef TIMER_H
#define TIMER_H
#include "../include/noncopyable.h"
#include "../include/MutexLock.hpp"
#include "HttpData.h"
#include <unistd.h>
#include <memory>
#include <queue>
#include <deque>


class HttpData;

class TimerNode
{
public:
	TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
	~TimerNode();
	TimerNode(TimerNode &tn);
	void update(int timeout);
	bool isValid();
	void clearReq();
	void setDeleted() { deleted_ = true; }
	bool isDeleted() const { return deleted_; }
	size_t getExpTime() const { return expiredTime_; }

private:
	bool deleted_;
	size_t expiredTime_;
	std::shared_ptr<HttpData> spHttpData;
};

//root node is the smallest element
struct TimerCmp
{
	bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) const
	{
		return a->getExpTime() > b->getExpTime();
	}
};

class TimerManager
{
public:
	TimerManager();
	~TimerManager();
	void addTimer(std::shared_ptr<HttpData> spHttpData, int timeout);
	void handleExpiredEvent();
private:
	typedef std::shared_ptr<TimerNode> spTimerNode;
	std::priority_queue<spTimerNode, std::deque<spTimerNode>, TimerCmp> timerNodeQueue;
};


#endif

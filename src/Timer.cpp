#include "Timer.h"
#include <sys/time.h>

TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout) : deleted_(false), spHttpData(requestData)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	expiredTime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

TimerNode::~TimerNode()
{
	if (spHttpData)
		spHttpData->handleClose();
}

TimerNode::TimerNode(TimerNode &tn) : spHttpData(tn.spHttpData)
{ }

void TimerNode::update(int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	expiredTime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool TimerNode::isValid()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));

	if (temp < expiredTime_)
		return true;
	else
	{
		this->setDeleted();
		return false;
	}
}

void TimerNode::clearReq()
{
	spHttpData.reset();
	this->setDeleted();
}

TimerManager::TimerManager()
{
	
}

TimerManager::~TimerManager()
{
	
}

void TimerManager::addTimer(std::shared_ptr<HttpData> spHttpData, int timeout)
{
	spTimerNode newNode(new TimerNode(spHttpData, timeout));
	timerNodeQueue.push(newNode);
	spHttpData->linkTimer(newNode);
}

void TimerManager::handleExpiredEvent()
{
	while (!timerNodeQueue.empty())
	{
		spTimerNode ptimer_now = timerNodeQueue.top();
		if (ptimer_now->isDeleted())
			timerNodeQueue.pop();
		else if (ptimer_now->isValid() == false)
			timerNodeQueue.pop();
		else
			break;
	}
}

#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "noncopyable.h"
#include "MutexLock.hpp"
#include "Condition.hpp"


class CountDownLatch : noncopyable
{
public:
	explicit CountDownLatch(int count);
	~CountDownLatch();
	void wait();
	void countDown();
private:
	mutable MutexLock mutex_;
	Condition cond_;
	int count_;
};








#endif

#include "CountDownLatch.h"
#include <iostream>

CountDownLatch::CountDownLatch(int count) : mutex_(), cond_(mutex_), count_(count)
{ }

CountDownLatch::~CountDownLatch()
{ }

void CountDownLatch::wait()
{
	MutexLockGuard lock(mutex_);
	while (count_ > 0)
		cond_.wait();
}

void CountDownLatch::countDown()
{
	MutexLockGuard lock(mutex_);
	--count_;

	if (count_ == 0)
		cond_.notifyAll();
}

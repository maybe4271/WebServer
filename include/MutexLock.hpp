#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H
#include "noncopyable.h"
//#include "Thread.h"

#include <sys/syscall.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>


class MutexLock : noncopyable
{
public:
	MutexLock() : holder_(0)
	{
		pthread_mutex_init(&mutex_, NULL);
	}

	~MutexLock()
	{
		assert(holder_ == 0);
		pthread_mutex_destroy(&mutex_);
	}

	bool isLockedByThisThred()
	{
		//return holder_ == gettid();
		return holder_ == syscall(SYS_gettid);
	}

	void lock()
	{
		pthread_mutex_lock(&mutex_);
		//holder_ = gettid();
		holder_ = syscall(SYS_gettid);
	}

	void unlock()
	{
		holder_ = 0;
		pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t *getPthreadMutex()
	{
		return &mutex_;
	}
private:
	pthread_mutex_t mutex_;
	friend class Condition;
	pid_t holder_;
};


class MutexLockGuard : noncopyable
{
public:
	explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex)
	{
		mutex_.lock();
	}
	~MutexLockGuard()
	{
		mutex_.unlock();
	}
private:
	MutexLock& mutex_;
};
#endif

#include <type_traits>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <memory>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include "CountDownLatch.h"
#include "Thread.h"
#include "CurrentThread.h"
#include "AsyncLogging.h"

namespace CurrentThread
{
	__thread int t_cachedTid = 0;
	__thread char t_tidString[32];
	__thread int t_tidStringLength = 6;
	__thread const char *t_threadName = "unknown";
	static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");
}

pid_t gettid()
{
	return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid()
{
	if (t_cachedTid == 0)
	{
		t_cachedTid = gettid();
		t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d", t_cachedTid);
	}
}

struct ThreadData
{
	typedef Thread::ThreadFunc ThreadFunc;
	ThreadFunc func_;
	std::string name_;
	pid_t *tid_;
	CountDownLatch *latch_;

	ThreadData(ThreadFunc func, const std::string& name, pid_t *tid, CountDownLatch *latch) : 
			func_(func), name_(name), tid_(tid), latch_(latch)
	{

	}
	
	void runInThread()
	{
		*tid_ = CurrentThread::tid();
		tid_ = NULL;
		latch_->countDown();
		latch_ = NULL;
		
		CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
		//change child thread name
		prctl(PR_SET_NAME, CurrentThread::t_threadName);

		//do Callback function
		func_();
		
		//omit exception handling
		CurrentThread::t_threadName = "finished";
	}

};

//main thread function
void *startThread(void *obj)
{
	ThreadData* data = static_cast<ThreadData*>(obj);
	data->runInThread();
	delete data;
}

Thread::Thread(ThreadFunc func, const std::string& name)
		:started_(false),
		joined_(false),
		pthreadId_(0),
		tid_(0),
		func_(std::move(func)),
		name_(name),
		latch_(1)
{
	setDefaultName();
}

Thread::~Thread()
{
	if (started_ && !joined_)
		pthread_detach(pthreadId_);
}

void Thread::setDefaultName()
{
	if(name_.empty())
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "Thread");
		name_ = buf;
	}
}

//start only once
void Thread::start()
{
	assert(!started_);
	started_ = true;
	ThreadData *data = new ThreadData(func_, name_, &tid_, &latch_);
	if (pthread_create(&pthreadId_, NULL, &startThread, data) < 0)
	{
		started_ = false;
		delete data;
		perror("pthread create error!");
	}
	else
	{
		latch_.wait();
		assert(tid_>0);
	}
}

int Thread::join()
{
	assert(started_);
	assert(!joined_);
	joined_ = true;
	return pthread_join(pthreadId_, NULL);
}









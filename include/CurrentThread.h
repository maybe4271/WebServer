#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

namespace CurrentThread 
{ 
	extern __thread int t_cachedTid; 
	extern __thread char t_tidString[32]; 
	extern __thread int t_tidStringLength; 
	extern __thread const char *t_threadName; 
	void cacheTid();

	inline int tid() 
	{ 
		if (t_cachedTid == 0) 
		{
			//t_cachedTid = ::syscall(SYS_gettid); 
			cacheTid();
		} 
		return t_cachedTid; 
	}

	inline const char* tidString()
	{
		return t_tidString;
	}

	inline int tidStringLength()
	{
		return t_tidStringLength;
	}

	inline const char *name() 
	{ 
		return t_threadName; 
	} 

	inline bool isMainThread() 
	{ 
		return tid() == ::getpid(); 
	} 
} 


#endif

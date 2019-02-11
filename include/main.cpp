#include <iostream>
#include <functional>

#include "Thread.h"
#include "Logging.h"
#include "AsyncLogging.h"

int main()
{
	LOG<<"HelloWorld";
	//AsyncLogging Async("test.txt", 2);
	//Async.start();
	//Async.append("haha", 4);
	//Async.stop();
	sleep(3);
	return 0;
}

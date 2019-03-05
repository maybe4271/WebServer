#include "EventLoop.h"
#include "Server.h"
#include "../include/Logging.h"

#include <getopt.h>
#include <string>

#define threadNum 4
#define port 80

int main(int argc, char *argv[])
{
	EventLoop mainLoop;
	Server myHTTPServer(&mainLoop, threadNum, port);
	myHTTPServer.start();
	mainLoop.loop();

	return 0;
}

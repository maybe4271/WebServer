#include "Util.h"
#include "../include/Logging.h"

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

const int MAX_BUFF = 4096;
ssize_t readn(int fd, void *buff, size_t n)
{
	size_t nleft = n;
	ssize_t nread = 0;
	ssize_t readSum = 0;
	char *ptr = (char *)buff;

	while (nleft > 0)
	{
		if((nread = read(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
				nread = 0;
			else if (errno == EAGAIN)
			{
				return nread;
			}
			else
			{
				LOG << "readn error!";
				return -1;
			}
		}
		else if (nread == 0)
		{
			break;
		}
		readSum += nread;
		nleft -= nread;
		ptr += nread;
	}
	return readSum;
}

ssize_t readn(int fd, std::string &inBuffer, bool zero)
{
	ssize_t nread = 0;
	ssize_t readSum = 0;

	while (1)
	{
		char buff[MAX_BUFF];
		if ((nread = read(fd, buff, MAX_BUFF)) < 0)
		{
			if (errno == EINTR)
				continue;
			else if (errno == EAGAIN)
			{
				return nread;
			}
			else
			{
				LOG << "readn error!";
				return -1;
			}
		}
		else if (nread == 0)
		{
			zero = true;
			break;
		}
		readSum += nread;
		inBuffer += std::string(buff, buff + nread);
	}
	return readSum;
}
ssize_t readn(int fd, std::string &inBuffer)
{
	ssize_t nread = 0;
	ssize_t readSum = 0;

	while (1)
	{
		char buff[MAX_BUFF];
		if ((nread = read(fd, buff, MAX_BUFF)) < 0)
		{
			if (errno == EINTR)
				continue;
			else if (errno == EAGAIN)
			{
				return nread;
			}
			else
			{
				LOG << "readn error!";
				return -1;
			}
		}
		else if (nread == 0)
		{
			break;
		}
		readSum += nread;
		inBuffer += std::string(buff, buff + nread);
	}
	return readSum;
}

ssize_t writen(int fd, void *buff, size_t n)
{
	size_t nleft = n;
	ssize_t nwrite = 0;
	ssize_t writeSum = 0;
	const char *ptr = (char *)buff;
	while (nleft > 0)
	{
		if ((nwrite = write(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				nwrite = 0;
				continue;
			}
			else if (errno == EAGAIN)
			{
				break;
			}
			else
			{
				LOG << "writen error!";
				return -1;
			}
		}
		writeSum += nwrite;
		nleft -= nwrite;
		ptr += nwrite;
	}
	
	return writeSum;
}

ssize_t writen(int fd, std::string &outBuffer)
{
	size_t nleft = outBuffer.size();
	ssize_t nwrite = 0;
	ssize_t writeSum = 0;
	const char *ptr = outBuffer.c_str();
	while (nleft > 0)
	{
		if ((nwrite = write(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				nwrite = 0;
				continue;
			}
			else if (errno == EAGAIN)
			{
				break;
			}
			else
			{
				LOG << "writen error!";
				return -1;
			}
		}
		writeSum += nwrite;
		nleft -= nwrite;
		ptr += nwrite;
	}
	if (writeSum == static_cast<int>(outBuffer.size()))
		outBuffer.clear();
	else
		outBuffer = outBuffer.substr(writeSum);
	return writeSum;
}

void handle_for_sigpipe()
{
	struct sigaction sa;
	memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	if (sigaction(SIGPIPE, &sa, NULL))
		return;
}

int setNonBlocking(int fd)
{
	int flag = fcntl(fd, F_GETFL, 0);
	if (flag == -1)
		return -1;

	flag |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flag) == -1)
		return -1;

	return 0;
}

void setSocketNoDelay(int fd)
{
	int enable = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
}

void setSocketNoLinger(int fd)
{
	struct linger linger_;
	linger_.l_onoff = 1;
	linger_.l_linger = 30;
	setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&linger_, sizeof(linger_));
}

void shutDownWR(int fd)
{
	shutdown(fd, SHUT_WR);
}

int socket_bind_listen(int port)
{
	if (port < 0 || port > 65535)
	{
		LOG << "port input error!";
		return -1;
	}

	int listen_fd = 0;
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		LOG << "fd create error!";
		return -1;
	}

	//Setting port multiplexing
	int optval = 1;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		return -1;
	
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family =AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons((unsigned short)port);

	if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		LOG << "bind error!";
		return -1;
	}

	if (listen(listen_fd, 2048) < 0)
	{
		LOG << "listen error!";
		return -1;
	}

	return listen_fd;

}

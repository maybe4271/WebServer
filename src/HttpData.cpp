#include "HttpData.h"
#include "Channel.h"
#include "Util.h"
#include "EventLoop.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <iostream>
#include <time.h>


pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;

const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;
const int DEFAULT_EXPIRED_TIME = 2000;
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000;


char favicon[555] = {
		'\x89', 'P', 'N', 'G', '\xD', '\xA', '\x1A', '\xA',
		'\x0', '\x0', '\x0', '\xD', 'I', 'H', 'D', 'R',
		'\x0', '\x0', '\x0', '\x10', '\x0', '\x0', '\x0', '\x10',
		'\x8', '\x6', '\x0', '\x0', '\x0', '\x1F', '\xF3', '\xFF',
		'a', '\x0', '\x0', '\x0', '\x19', 't', 'E', 'X',
		't', 'S', 'o', 'f', 't', 'w', 'a', 'r',
		'e', '\x0', 'A', 'd', 'o', 'b', 'e', '\x20',
		'I', 'm', 'a', 'g', 'e', 'R', 'e', 'a',
		'd', 'y', 'q', '\xC9', 'e', '\x3C', '\x0', '\x0',
		'\x1', '\xCD', 'I', 'D', 'A', 'T', 'x', '\xDA',
		'\x94', '\x93', '9', 'H', '\x3', 'A', '\x14', '\x86',
		'\xFF', '\x5D', 'b', '\xA7', '\x4', 'R', '\xC4', 'm',
		'\x22', '\x1E', '\xA0', 'F', '\x24', '\x8', '\x16', '\x16',
		'v', '\xA', '6', '\xBA', 'J', '\x9A', '\x80', '\x8',
		'A', '\xB4', 'q', '\x85', 'X', '\x89', 'G', '\xB0',
		'I', '\xA9', 'Q', '\x24', '\xCD', '\xA6', '\x8', '\xA4',
		'H', 'c', '\x91', 'B', '\xB', '\xAF', 'V', '\xC1',
		'F', '\xB4', '\x15', '\xCF', '\x22', 'X', '\x98', '\xB',
		'T', 'H', '\x8A', 'd', '\x93', '\x8D', '\xFB', 'F',
		'g', '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f', 'v',
		'f', '\xDF', '\x7C', '\xEF', '\xE7', 'g', 'F', '\xA8',
		'\xD5', 'j', 'H', '\x24', '\x12', '\x2A', '\x0', '\x5',
		'\xBF', 'G', '\xD4', '\xEF', '\xF7', '\x2F', '6', '\xEC',
		'\x12', '\x20', '\x1E', '\x8F', '\xD7', '\xAA', '\xD5', '\xEA',
		'\xAF', 'I', '5', 'F', '\xAA', 'T', '\x5F', '\x9F',
		'\x22', 'A', '\x2A', '\x95', '\xA', '\x83', '\xE5', 'r',
		'9', 'd', '\xB3', 'Y', '\x96', '\x99', 'L', '\x6',
		'\xE9', 't', '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',
		'\xA7', '\xC4', 'b', '1', '\xB5', '\x5E', '\x0', '\x3',
		'h', '\x9A', '\xC6', '\x16', '\x82', '\x20', 'X', 'R',
		'\x14', 'E', '6', 'S', '\x94', '\xCB', 'e', 'x',
		'\xBD', '\x5E', '\xAA', 'U', 'T', '\x23', 'L', '\xC0',
		'\xE0', '\xE2', '\xC1', '\x8F', '\x0', '\x9E', '\xBC', '\x9',
		'A', '\x7C', '\x3E', '\x1F', '\x83', 'D', '\x22', '\x11',
		'\xD5', 'T', '\x40', '\x3F', '8', '\x80', 'w', '\xE5',
		'3', '\x7', '\xB8', '\x5C', '\x2E', 'H', '\x92', '\x4',
		'\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g', '\x98',
		'\xE9', '6', '\x1A', '\xA6', 'g', '\x15', '\x4', '\xE3',
		'\xD7', '\xC8', '\xBD', '\x15', '\xE1', 'i', '\xB7', 'C',
		'\xAB', '\xEA', 'x', '\x2F', 'j', 'X', '\x92', '\xBB',
		'\x18', '\x20', '\x9F', '\xCF', '3', '\xC3', '\xB8', '\xE9',
		'N', '\xA7', '\xD3', 'l', 'J', '\x0', 'i', '6',
		'\x7C', '\x8E', '\xE1', '\xFE', 'V', '\x84', '\xE7', '\x3C',
		'\x9F', 'r', '\x2B', '\x3A', 'B', '\x7B', '7', 'f',
		'w', '\xAE', '\x8E', '\xE', '\xF3', '\xBD', 'R', '\xA9',
		'd', '\x2', 'B', '\xAF', '\x85', '2', 'f', 'F',
		'\xBA', '\xC', '\xD9', '\x9F', '\x1D', '\x9A', 'l', '\x22',
		'\xE6', '\xC7', '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15',
		'\x90', '\x7', '\x93', '\xA2', '\x28', '\xA0', 'S', 'j',
		'\xB1', '\xB8', '\xDF', '\x29', '5', 'C', '\xE', '\x3F',
		'X', '\xFC', '\x98', '\xDA', 'y', 'j', 'P', '\x40',
		'\x0', '\x87', '\xAE', '\x1B', '\x17', 'B', '\xB4', '\x3A',
		'\x3F', '\xBE', 'y', '\xC7', '\xA', '\x26', '\xB6', '\xEE',
		'\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
		'\xA', '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X', '\x0',
		'\x27', '\xEB', 'n', 'V', 'p', '\xBC', '\xD6', '\xCB',
		'\xD6', 'G', '\xAB', '\x3D', 'l', '\x7D', '\xB8', '\xD2',
		'\xDD', '\xA0', '\x60', '\x83', '\xBA', '\xEF', '\x5F', '\xA4',
		'\xEA', '\xCC', '\x2', 'N', '\xAE', '\x5E', 'p', '\x1A',
		'\xEC', '\xB3', '\x40', '9', '\xAC', '\xFE', '\xF2', '\x91',
		'\x89', 'g', '\x91', '\x85', '\x21', '\xA8', '\x87', '\xB7',
		'X', '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N', 'N',
		'b', 't', '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
		'\xEC', '\x86', '\x2', 'H', '\x26', '\x93', '\xD0', 'u',
		'\x1D', '\x7F', '\x9', '2', '\x95', '\xBF', '\x1F', '\xDB',
		'\xD7', 'c', '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF',
		'\x22', 'J', '\xC3', '\x87', '\x0', '\x3', '\x0', 'K',
		'\xBB', '\xF8', '\xD6', '\x2A', 'v', '\x98', 'I', '\x0',
		'\x0', '\x0', '\x0', 'I', 'E', 'N', 'D', '\xAE',
		'B', '\x60', '\x82',
};

void MimeType::init()
{
		mime[".html"] = "text/html";
		mime[".avi"] = "video/x-msvideo";
		mime[".bmp"] = "image/bmp";
		mime[".c"] = "text/plain";
		mime[".doc"] = "application/msword";
		mime[".gif"] = "image/gif";
		mime[".gz"] = "application/x-gzip";
		mime[".htm"] = "text/html";
		mime[".ico"] = "image/x-icon";
		mime[".jpg"] = "image/jpeg";
		mime[".png"] = "image/png";
		mime[".txt"] = "text/plain";
		mime[".mp3"] = "audio/mp3";
		mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string &suffix)
{
	pthread_once(&once_control, MimeType::init);
	if (mime.find(suffix) == mime.end())
		return mime["default"];
	else
		return mime[suffix];
}

HttpData::HttpData(EventLoop *loop, int connfd) : loop_(loop), channel_(new Channel(loop, connfd)), fd_(connfd), error_(false), connectionState_(H_CONNECTED), method_(METHOD_GET), HTTPVersion_(HTTP_11), nowReadPos_(0), state_(STATE_PARSE_URI), hState_(H_START), keepAlive_(false)
{
	channel_->setReadHandler(bind(&HttpData::handleRead, this));
	channel_->setWriteHandler(bind(&HttpData::handleWrite, this));
	channel_->setConnHandler(bind(&HttpData::handleConn, this));
}

void HttpData::reset()
{
	fileName_.clear();
	path_.clear();
	nowReadPos_ = 0;
	state_ = STATE_PARSE_URI;
	hState_ = H_START;
	headers_.clear();

	if (timer_.lock())
	{
		std::shared_ptr<TimerNode> my_timer(timer_.lock());
		my_timer->clearReq();
		timer_.reset();
	}
}

void HttpData::seperateTimer()
{
	if (timer_.lock())
	{
		std::shared_ptr<TimerNode> my_timer(timer_.lock());
		my_timer->clearReq();
		timer_.reset();
	}
}

void HttpData::handleRead()
{
	__uint32_t &events_ = channel_->getEvents();
	do {
		bool zero = false;
		int read_num = readn(fd_, inBuffer_, zero);
		LOG << "Request:" << inBuffer_;
		if (connectionState_ == H_DISCONNECTING)
		{
			inBuffer_.clear();
			break;
		}

		if (read_num < 0)
		{
			perror("1");
			error_ = true;
			handleError(fd_, 400, "Bad Request");
			break;
		}
		else if (zero)
		{
			connectionState_ = H_DISCONNECTING;
			if (read_num == 0)
			{
				std::cout << "read_num == 0" << std::endl;
				break;
			}
		}

		if (state_ == STATE_PARSE_URI)
		{
			URIState flag = this->parseURI();
			if (flag == PARSE_URI_AGAIN)
				break;
			else if (flag == PARSE_URI_ERROR)
			{
				perror("state_ == STATE_PARSE_URI");
				LOG << "FD=" << fd_ << "," << inBuffer_;
				inBuffer_.clear();
				error_ = true;
				handleError(fd_, 400, "Bad Request");
				break;
			}
			else
				state_ = STATE_PARSE_HEADERS;
		}

		if (state_ == STATE_PARSE_HEADERS)
		{
			HeaderState flag = this->parseHeaders();
			if (flag == PARSE_HEADER_AGAIN)
				break;
			else if (flag == PARSE_HEADER_ERROR)
			{
				perror("state_ == STATE_PARSE_HEADERS");
				error_ = true;
				handleError(fd_, 400, "Bad Request");
				break;
			}
			if (method_ == METHOD_POST)
			{
				state_ = STATE_RECV_BODY;
			}
			else
			{
				state_ = STATE_ANALYSIS;
			}
		}

		if (state_ == STATE_RECV_BODY)
		{
			int content_length = -1;
			if (headers_.find("Content-length") != headers_.end())
			{
				content_length = stoi(headers_["Content-length"]);
			}
			else
			{
				error_ = true;
				handleError(fd_, 400, "Bad Request: Lack of argument (Content-length)");
				break;
			}
			if (static_cast<int>(inBuffer_.size()) < content_length)
			{
				break;
			}
			state_ = STATE_ANALYSIS;
		}

		if (state_ == STATE_ANALYSIS)
		{
			AnalysisState flag = this->analysisRequest();
			if (flag == ANALYSIS_SUCCESS)
			{
				state_ = STATE_FINISH;
				break;
			}
			else
			{
				error_ = true;
				break;
			}
		}
	} while (0);

	if (!error_)
	{
		if (!outBuffer_.size() > 0)
		{
			handleWrite();
			//events_ |= EPOLLOUT;
		}
		if (!error_ && state_ == STATE_FINISH)
		{
			this->reset();
			if (inBuffer_.size() > 0)
			{
				if (connectionState_ != H_DISCONNECTING)
					handleRead();
			}

		}
		else if (!error_ && connectionState_ != H_DISCONNECTED)
			events_ |= EPOLLIN;
	}
}

void HttpData::handleWrite()
{
	if (!error_ && connectionState_ != H_DISCONNECTED)
	{
		__uint32_t &events_ = channel_->getEvents();
		if (writen(fd_, outBuffer_) < 0)
		{
			perror("writen");
			events_ = 0;
			error_ = true;
		}
		if (outBuffer_.size() > 0)
			events_ |= EPOLLOUT;
	}
}

void HttpData::handleConn()
{
	seperateTimer();
	__uint32_t &events_ = channel_->getEvents();
	if (!error_ && connectionState_ == H_CONNECTED)
	{
		if (events_ != 0)
		{
			int timeout = DEFAULT_EXPIRED_TIME;
			if (keepAlive_)
				timeout = DEFAULT_KEEP_ALIVE_TIME;
			if ((events_ & EPOLLIN) && (events_ & EPOLLOUT))
			{
				events_ = __uint32_t(0);
				events_ |= EPOLLOUT;
			}
			events_ |= EPOLLET;
			loop_->updatePoller(channel_, timeout);
		}
		else if (keepAlive_)
		{
			events_ |= (EPOLLIN | EPOLLET);
			int timeout = DEFAULT_KEEP_ALIVE_TIME;
			loop_->updatePoller(channel_, timeout);
		}
		else
		{
			events_ |= (EPOLLIN | EPOLLET);
			int timeout = (DEFAULT_KEEP_ALIVE_TIME >> 1);
			loop_->updatePoller(channel_, timeout);
		}
	}
	else if (!error_ && connectionState_ == H_DISCONNECTING && (events_ & EPOLLOUT))
	{
		events_ = (EPOLLOUT | EPOLLET);
	}
	else
	{
		loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
	}
}

URIState HttpData::parseURI()
{
	std::string &str = inBuffer_;
	std::string cop = str;

	size_t pos = str.find('\r', nowReadPos_);
	if (pos < 0)
	{
		return PARSE_URI_AGAIN;
	}

	std::string request_line = str.substr(0, pos);
	if (str.size() > pos + 1)
	{
		str = str.substr(pos + 1);
	}
	else
	{
		str.clear();
	}

	int posGet = request_line.find("GET");
	int posPost = request_line.find("POST");
	int posHead = request_line.find("HEAD");

	if (posGet >= 0)
	{
		pos = posGet;
		method_ = METHOD_GET;
	}
	else if (posPost >= 0)
	{
		pos = posPost;
		method_ = METHOD_POST;
	}
	else if (posHead >= 0)
	{
		pos = posHead;
		method_ = METHOD_HEAD;
	}
	else
	{
		return PARSE_URI_ERROR;
	}

	pos = request_line.find("/", pos);
	if (pos < 0)
	{
		fileName_ = "index.html";
		HTTPVersion_ = HTTP_11;
		return PARSE_URI_SUCCESS;
	}
	else
	{
		size_t _pos = request_line.find(' ', pos);

	}
}

HeaderState HttpData::parseHeaders()
{
	
}

AnalysisState HttpData::analysisRequest()
{
	
}

void HttpData::handleError(int fd, int err_num, std::string short_msg)
{
	
}

void HttpData::handleClose()
{
	connectionState_ = H_DISCONNECTED;
	std::shared_ptr<HttpData> guard(shared_from_this());
	loop_->removeFromPoller(channel_);
}

void HttpData::newEvent()
{
	channel_->setEvents(DEFAULT_EVENT);
	loop_->addToPoller(channel_, DEFAULT_EXPIRED_TIME);
}

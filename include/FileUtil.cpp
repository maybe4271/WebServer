#include "FileUtil.h"

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

AppendFile::AppendFile(std::string filename) : fp_(::fopen(filename.c_str(), "ae")) //'e' for O_CLOEXEC
{
	assert(fp_);
	::setbuffer(fp_, buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile()
{
	::fclose(fp_);
}

/*
//maybe wrong, but more concise.
void AppendFile::append(const char *logline, size_t len)
{
	size_t n = 0;
	do {
		size_t x = this->write(logline+n, len);
		if (x == 0)
		{
			int err = ferror(fp_);
			if (err)
				fprintf(stderr, "AppendFile::append() falied!\n");
			break;
		}
		n += x;
		len -= n;
	} while (len > 0)
}
*/

void AppendFile::append(const char *logline, const size_t len)
{
	size_t n = this->write(logline, len);
	size_t remain = len - n;

	while (remain > 0)
	{
		size_t x = this->write(logline + n, remain);
		if (x == 0)
		{
			int err = ferror(fp_);
			if (err)
				fprintf(stderr, "AppendFile::append() falied!\n");
			break;
		}
		n += x;
		remain = len - n;
	}
}

void AppendFile::flush()
{
	::fflush(fp_);
}

size_t AppendFile::write(const char *logline, size_t len)
{
	//Thread safe, For details: http://www.tin.org/bin/man.cgi?section=3&topic=fwrite_unlocked
	return ::fwrite_unlocked(logline, 1, len, fp_);
}


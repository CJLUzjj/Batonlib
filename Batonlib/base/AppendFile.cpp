//by Zhujunjie
#include "AppendFile.h"



AppendFile::AppendFile(std::string filename)
	:fp_(fopen(filename.c_str(), "ae"))
	//:fp_(stdout)
{
	//用户提供缓冲区
	setbuffer(fp_, buffer_, sizeof buffer_);
	//std::cout << "AppendFile:fp_ is " << fp_ << std::endl;
}


AppendFile::~AppendFile()
{
	fclose(fp_);
}

void AppendFile::append(const char *line, size_t len)
{
	size_t n = this->write(line, len);
	size_t remain = len - n;
	while (remain > 0) {
		size_t x = this->write(line, len);
		if (x == 0) {
			int err = ferror(fp_);
			if(err)
				fprintf(stderr, "AppendFile::append() failed !\n");
			break;
		}
		remain -= x;
	}
}

void AppendFile::flush()
{
	fflush(fp_);
}

size_t AppendFile::write(const char *line, size_t len)
{
	return fwrite_unlocked(line, 1, len, fp_);
}
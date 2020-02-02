//by Zhujunjie
#include "LogFile.h"



LogFile::LogFile(std::string basename, int Every_n_flush)
	:basename_(basename),
	 Every_n_flush_(Every_n_flush),
	 count_(0),
	 ptr_mutex_(new Mutex)
{
	file_.reset(new AppendFile(basename_));
	//std::cout << "LogFile:basename_ is " << basename_ << std::endl;
}


LogFile::~LogFile()
{
}

void LogFile::append(const char *line, int len)
{
	MutexLockGuard lock(*ptr_mutex_);
	append_unlock(line, len);
}

void LogFile::append_unlock(const char *line, int len)
{
	file_->append(line, len);
	++count_;
	if (count_ >= Every_n_flush_) {
		flush();
		count_ = 0;
	}
}

void LogFile::flush()
{
	MutexLockGuard lock(*ptr_mutex_);
	file_->flush();
}
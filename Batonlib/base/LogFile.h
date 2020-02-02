//by Zhujunjie
#pragma once

#include "Mutex.h"
#include "AppendFile.h"
#include <string>
#include <memory>
#include <iostream>
class LogFile
{
public:
	LogFile(std::string basename, int Every_n_flush = 1024);
	~LogFile();
	void append(const char *line, int len);
	void flush();
private:
	void append_unlock(const char *line, int len);
	int Every_n_flush_;
	const std::string basename_;
	int count_;
	std::unique_ptr<Mutex> ptr_mutex_;
	std::unique_ptr<AppendFile> file_;
};


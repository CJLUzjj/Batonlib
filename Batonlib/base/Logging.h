//by Zhujunjie
#pragma once

#include <string>
#include <time.h>
#include <sys/time.h> 
#include "AsyncLogging.h"
#include "LogStream.h"
#include "Thread.h"

class AsyncLogging;

class Logging
{
public:
	Logging(const char* filename, int line);
	~Logging();
	LogStream& stream();
	static void setLogFileName(std::string logfilename) { LogFileName_ = logfilename; }
	static std::string getLogFileName() { return LogFileName_; }
private:
	class Impl
	{
	public:
		Impl(const char *filename, int line);
		void formatTime();

		LogStream stream_;
		int line_;
		std::string basename_;
	};
	Impl impl_;
	static std::string LogFileName_;
};

#define LOG Logging(__FILE__,__LINE__).stream()
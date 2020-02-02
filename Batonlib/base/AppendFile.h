//by Zhujunjie
#pragma once

#include <string>
#include <iostream>
class AppendFile
{
public:
	AppendFile(std::string filename);
	~AppendFile();
	void append(const char *line, size_t len);
	void flush();
private:
	size_t write(const char *line, size_t len);
	FILE *fp_;
	char buffer_[1024 * 64];//64KB
};


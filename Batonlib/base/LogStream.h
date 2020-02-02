//by Zhujunjie
#pragma once
#include <assert.h>
#include <string.h>
#include <string>
#include <algorithm>

#define KSMALLBUFFER 4000

class Log_Buffer
{
public:
	Log_Buffer();
	~Log_Buffer();
	const char* data() const;
	int length() const;

	char* current();
	int avail() const;
	void add(size_t len);

	void reset();
	void bzero();

	void append(const char* buf, size_t len);

private:
	const char* end() const;
	char data_[KSMALLBUFFER];
	char* cur_;
};


class LogStream
{
public:
	LogStream();
	~LogStream();
	LogStream& operator<<(bool v)
	{
		buffer_.append(v ? "1" : "0", 1);
		return *this;
	}

	LogStream& operator<<(short);
	LogStream& operator<<(unsigned short);
	LogStream& operator<<(int);
	LogStream& operator<<(unsigned int);
	LogStream& operator<<(long);
	LogStream& operator<<(unsigned long);
	LogStream& operator<<(long long);
	LogStream& operator<<(unsigned long long);

	LogStream& operator<<(const void*);

	LogStream& operator<<(float v)
	{
		*this << static_cast<double>(v);
		return *this;
	}
	LogStream& operator<<(double);
	LogStream& operator<<(long double);

	LogStream& operator<<(char v)
	{
		buffer_.append(&v, 1);
		return *this;
	}

	LogStream& operator<<(const char* str)
	{
		if (str)
			buffer_.append(str, strlen(str));
		else
			buffer_.append("(null)", 6);
		return *this;
	}

	LogStream& operator<<(const unsigned char* str)
	{
		return operator<<(reinterpret_cast<const char*>(str));
	}

	LogStream& operator<<(const std::string& v)
	{
		buffer_.append(v.c_str(), v.size());
		return *this;
	}

	void append(const char* data, int len) { buffer_.append(data, len); }
	const Log_Buffer& buffer() const { return buffer_; }
	void resetBuffer() { buffer_.reset(); }
private:
	template<typename T>
	void formatInteger(T);
	Log_Buffer buffer_;
	static const int kMaxNumericSize = 32;
};


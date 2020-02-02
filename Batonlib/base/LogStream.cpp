//by Zhujunjie
#include "LogStream.h"

//Log_Buffer

Log_Buffer::Log_Buffer()
	:cur_(data_)
{
}

Log_Buffer::~Log_Buffer()
{
}

const char* Log_Buffer::data() const { return data_; }
int Log_Buffer::length() const { return static_cast<int>(cur_ - data_); }

char* Log_Buffer::current() { return cur_; }
int Log_Buffer::avail() const { return static_cast<int>(end() - cur_); }
void Log_Buffer::add(size_t len) { cur_ += len; }

void Log_Buffer::reset() { cur_ = data_; }
void Log_Buffer::bzero() { memset(data_, 0, sizeof data_); }
const char* Log_Buffer::end() const { return data_ + sizeof data_; }

void Log_Buffer::append(const char* buf, size_t len)
{
	if (avail() > len) {
		memcpy(cur_, buf, len);
		cur_ += len;
	}
}


//LogStream

//from muduo
//将整型变为字符串类型

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

const char digitsHex[] = "0123456789ABCDEF";

template<typename T>
size_t convert(char buf[], T value)
{
	T i = value;
	char *p = buf;

	do
	{
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);

	if (value < 0)
	{
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

LogStream::LogStream()
{
}


LogStream::~LogStream()
{
}

template<typename T>
void LogStream::formatInteger(T v)
{
	// buffer容不下kMaxNumericSize个字符的话会被直接丢弃
	if (buffer_.avail() >= kMaxNumericSize)
	{
		size_t len = convert(buffer_.current(), v);
		buffer_.add(len);
	}
}

LogStream& LogStream::operator<<(short v)
{
	*this << static_cast<int>(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
	*this << static_cast<unsigned int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(const void* v)
{
	uintptr_t val = reinterpret_cast<uintptr_t>(v);
	if(buffer_.avail() >= kMaxNumericSize)
	{
		char* buf = buffer_.current();
		buf[0] = '0';
		buf[1] = 'x';
		
		uintptr_t i = val;
		char* p = buf;

		do{
			int lsd = static_cast<int>(i % 16);
			i /= 16;
			*p++ = digitsHex[lsd];
		}while(i != 0);

		*p = '\0';
		std::reverse(buf, p);

		size_t len = p - buf;
		buffer_.add(len+2);
	}
	return *this;
}

LogStream& LogStream::operator<<(long long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(double v)
{
	if (buffer_.avail() >= kMaxNumericSize)
	{
		int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
		buffer_.add(len);
	}
	return *this;
}

LogStream& LogStream::operator<<(long double v)
{
	if (buffer_.avail() >= kMaxNumericSize)
	{
		int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
		buffer_.add(len);
	}
	return *this;
}
//by Zhujunjie
#include "Logging.h"

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging *AsyncLogger_;
std::string Logging::LogFileName_ = "zhujunjie_WebServer.log";

void thread_once()
{
	AsyncLogger_ = new AsyncLogging(Logging::getLogFileName());
	AsyncLogger_->start();
}

void output(const char* msg, int len)
{
	pthread_once(&once_control_, thread_once);
	AsyncLogger_->append(msg, len);
}

Logging::Impl::Impl(const char *filename, int line)
	:basename_(filename),
	 line_(line),
	 stream_()
{
	formatTime();

}

void Logging::Impl::formatTime()
{
	struct timeval tv;
	time_t time;
	char str_t[26] = { 0 };
	gettimeofday(&tv, NULL);
	time = tv.tv_sec;
	struct tm* p_time = localtime(&time);
	strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
	stream_ << str_t;
}

Logging::Logging(const char* filename, int line)
	:impl_(filename, line)
{
}

LogStream& Logging::stream()
{
	return impl_.stream_;
}


Logging::~Logging()
{
	impl_.stream_ << " -- " << impl_.basename_ << ':' << impl_.line_ << '\n';
	const Log_Buffer& buf(stream().buffer());
	output(buf.data(), buf.length());
}

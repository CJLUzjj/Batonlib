OBJS = time_wheel.o tool.o co_context.o co_epoll.o co_stack.o coroutine.o scheduler.o context_swap.o co_sys_call.o
BASEOBJS = Mutex.o Condition.o CountDownLatch.o Thread.o ThreadPool.o CurrentThread.o AppendFile.o AsyncLogging.o LogFile.o Logging.o LogStream.o
TEST_O = Zsingle_thread_test.o
TEST_CPP = Zsingle_thread_test.cpp
GPP = g++ -std=c++11 -g -c

server : $(TEST_O) $(BASEOBJS) $(OBJS)
		g++ -lpthread $(TEST_O) $(BASEOBJS) $(OBJS) -o server

$(TEST_O) : $(TEST_CPP)
		$(GPP) $(TEST_CPP)

time_wheel.o : time_wheel.cpp
		$(GPP) time_wheel.cpp

tool.o : tool.cpp
		$(GPP) tool.cpp

context_swap.o : context_swap.S
		gcc -c context_swap.S

co_context.o : co_context.cpp
		$(GPP) co_context.cpp

co_epoll.o : co_epoll.cpp
		$(GPP) co_epoll.cpp

co_stack.o : co_stack.cpp
		$(GPP) co_stack.cpp

coroutine.o : coroutine.cpp
		$(GPP) coroutine.cpp

scheduler.o : scheduler.cpp
		$(GPP) scheduler.cpp

co_sys_call.o : co_sys_call.cpp
		$(GPP) co_sys_call.cpp



Mutex.o : base/Mutex.cpp
	$(GPP) base/Mutex.cpp

Condition.o : base/Condition.cpp
	$(GPP) base/Condition.cpp

CountDownLatch.o : base/CountDownLatch.cpp
	$(GPP) base/CountDownLatch.cpp

Thread.o : base/Thread.cpp
	$(GPP) base/Thread.cpp

ThreadPool.o : base/ThreadPool.cpp
	$(GPP) base/ThreadPool.cpp

CurrentThread.o : base/CurrentThread.cpp
	$(GPP) base/CurrentThread.cpp

AppendFile.o : base/AppendFile.cpp
	$(GPP) base/AppendFile.cpp

AsyncLogging.o : base/AsyncLogging.cpp
	$(GPP) base/AsyncLogging.cpp

LogFile.o : base/LogFile.cpp
	$(GPP) base/LogFile.cpp

Logging.o : base/Logging.cpp
	$(GPP) base/Logging.cpp

LogStream.o : base/LogStream.cpp
	$(GPP) base/LogStream.cpp

clean:
		rm -rf *.o server

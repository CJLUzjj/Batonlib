all:chat.test cond.test echo_conn.test coroutine.test echo.test time.test co_chat.test

OBJS = time_wheel.o tool.o co_context.o co_epoll.o co_stack.o coroutine.o scheduler.o context_swap.o co_sys_call.o co_condition.o
BASEOBJS = Mutex.o Condition.o CountDownLatch.o Thread.o ThreadPool.o CurrentThread.o AppendFile.o AsyncLogging.o LogFile.o Logging.o LogStream.o
NETOBJS = connection.o server.o
TEST_O = Zchat_test.o
TEST_CPP = Zchat_test.cpp
GPP = g++ -std=c++11 -g -c

co_chat.test : Zco_chat_test.o $(BASEOBJS) $(OBJS) $(NETOBJS)
		g++ -lpthread Zco_chat_test.o $(BASEOBJS) $(OBJS) $(NETOBJS) -o co_chat.test

chat.test : Zchat_test.o $(BASEOBJS) $(OBJS) $(NETOBJS)
		g++ -lpthread Zchat_test.o $(BASEOBJS) $(OBJS) $(NETOBJS) -o chat.test

cond.test : Zcond_test.o $(BASEOBJS) $(OBJS) $(NETOBJS)
		g++ -lpthread Zcond_test.o $(BASEOBJS) $(OBJS) $(NETOBJS) -o cond.test

echo_conn.test : Zconn_echo_test.o $(BASEOBJS) $(OBJS) $(NETOBJS)
		g++ -lpthread Zconn_echo_test.o $(BASEOBJS) $(OBJS) $(NETOBJS) -o echo_conn.test

coroutine.test : Zcoroutine_test.o $(BASEOBJS) $(OBJS) $(NETOBJS)
		g++ -lpthread Zcoroutine_test.o $(BASEOBJS) $(OBJS) $(NETOBJS) -o coroutine.test

echo.test : Zecho_test.o $(BASEOBJS) $(OBJS) $(NETOBJS)
		g++ -lpthread Zecho_test.o $(BASEOBJS) $(OBJS) $(NETOBJS) -o echo.test

time.test : Ztime_test.o $(BASEOBJS) $(OBJS) $(NETOBJS)
		g++ -lpthread Ztime_test.o $(BASEOBJS) $(OBJS) $(NETOBJS) -o time.test

Zco_chat_test.o : Zco_chat_test.cpp
		$(GPP) Zco_chat_test.cpp

Zchat_test.o : Zchat_test.cpp
		$(GPP) Zchat_test.cpp

Zcond_test.o : Zcond_test.cpp
		$(GPP) Zcond_test.cpp

Zconn_echo_test.o : Zconn_echo_test.cpp
		$(GPP) Zconn_echo_test.cpp

Zcoroutine_test.o : Zcoroutine_test.cpp
		$(GPP) Zcoroutine_test.cpp

Zecho_test.o : Zecho_test.cpp
		$(GPP) Zecho_test.cpp

Ztime_test.o : Ztime_test.cpp
		$(GPP) Ztime_test.cpp



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

co_condition.o : co_condition.cpp
		$(GPP) co_condition.cpp



connection.o : net/connection.cpp
		$(GPP) net/connection.cpp

server.o : net/server.cpp
		$(GPP) net/server.cpp


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
		rm -rf *.o *.test *.log

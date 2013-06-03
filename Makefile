INS_PATH=/home/helen/lua/b5.1
INS_PATH=/data/home/guangxiang.feng/lua_test/b5.1

BOOST_INC_PATH=/home/helen/soft.comp/boost_1_53_0/
BOOST_LIB_PATH=/home/helen/soft.comp/boost_1_53_0/stage/lib


BOOST_INC_PATH=/usr/local/xce/include/boost-1_38/
BOOST_LIB_PATH=/usr/local/xce/lib



CC=gcc
CCFLAGS=-g -c -Wall -I$(INS_PATH)/include -I$(BOOST_INC_PATH)
LDFLAGS=-L$(INS_PATH)/lib -L$(BOOST_LIB_PATH)  -Wl,-E -llua -lm -ldl -lcrypto -lboost_thread -lboost_system
LDFLAGS=-L$(INS_PATH)/lib -L$(BOOST_LIB_PATH)  -Wl,-E -llua -lm -ldl -lcrypto -lboost_thread-gcc41-mt

SHAREFLAGS= #=-static

APP=test


all:$(APP)

$(APP):LuaServ.o ConfigId.o LuaState.o LuaPool.o int64.o test.o
	$(CC) LuaServ.o ConfigId.o LuaState.o LuaPool.o int64.o test.o $(LDFLAGS) -o test

ConfigId.o:ConfigId.h ConfigId.cpp LuaState.h
	$(CC) $(CCFLAGS) ConfigId.cpp 

LuaServ.o:LuaServ.h LuaServ.cpp LuaState.h ConfigId.h
	$(CC) $(CCFLAGS) LuaServ.cpp

LuaState.o:LuaState.h LuaState.cpp
	$(CC) $(CCFLAGS) LuaState.cpp

LuaPool.o:LuaPool.h LuaPool.cpp LuaServ.h LuaState.h
	$(CC) $(CCFLAGS) LuaPool.cpp

int64.o:LuaState.h int64.cpp
	$(CC) $(CCFLAGS) int64.cpp

test.o:test.cpp ConfigId.h LuaServ.h LuaPool.h LuaState.h
	$(CC) $(CCFLAGS) test.cpp


.PHONY: clean

clean:
	rm -f *.o test

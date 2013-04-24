SRCS = prompt_io.cpp test.cpp
HDRS = prompt_io.h
LINKS = -lboost_thread-mt
FLAGS = -Wall -std=c++0x
OUT = test

all: test

test: $(SRCS) $(HDRS)
	g++ $(FLAGS) $(SRCS) $(LINKS) -o $(OUT)
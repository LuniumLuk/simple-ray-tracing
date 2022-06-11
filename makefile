CXX 		:= g++
CFLAGS   	:= -g -std=c++11 -Wformat

TARGET		:= main

SOURCEDIR  	:= src
INCLUDES   	:= -I$(SOURCEDIR)
HEADERS    	:= $(wildcard $(addprefix $(SOURCEDIR)/, *.hpp))

all: $(TARGET)

run: all
	./$(TARGET)

help:
	@echo $(HEADERS)
	@echo $(INCLUDES)

$(TARGET): $(SOURCEDIR)/main.cpp $(HEADERS)
	@$(CXX) $(CFLAGS) $(INCLUDES) -o main.o -c $(SOURCEDIR)/main.cpp
	@$(CXX) $(CFLAGS) $(INCLUDES) -o $@ main.o
	@$(RM) main.o

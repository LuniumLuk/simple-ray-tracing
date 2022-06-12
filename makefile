CXX 		:= g++
CFLAGS   	:= -g -std=c++11 -Wformat

TARGET		:= main

SOURCEDIR  	:= src
INCLUDES   	:= -I$(SOURCEDIR)
HEADERS    	:= $(wildcard $(addprefix $(SOURCEDIR)/, *.hpp))

ifeq ($(OS),Windows_NT)
	RM	    := del
endif

all: $(TARGET)

run: all
	./$(TARGET)

$(TARGET): $(SOURCEDIR)/main.cpp $(HEADERS)
	@$(CXX) $(CFLAGS) $(INCLUDES) -o main.o -c $(SOURCEDIR)/main.cpp
	@$(CXX) $(CFLAGS) $(INCLUDES) -o $(TARGET) main.o
	@$(RM) main.o

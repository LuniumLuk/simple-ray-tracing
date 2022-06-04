CXX 		:= g++
CXX_FLAGS   := -g -std=c++20 -Wformat
CC			:= gcc

SRC 		:= ./src
INCLUDE 	:= ./include

EXECUTABLE	:= main

RM 			:= del

all: $(EXECUTABLE)

run: all
	./$(EXECUTABLE)

$(EXECUTABLE): $(SRC)/main.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@

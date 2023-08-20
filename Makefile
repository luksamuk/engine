CXX = g++ --std=c++20
CXXFLAGS = -g -Wall -I./include

OFILES =\
	obj/glad.o\
	obj/main.o

LIBS = -lGL -lglfw

BIN = bin/game

.PHONY: dirs

all: dirs $(OFILES)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OFILES) $(LIBS)

obj/glad.o: src/glad.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

dirs:
	@mkdir -p bin/ obj/


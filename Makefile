CXX = g++ --std=c++20
INCLUDES = -I./include
CXXFLAGS = -g -Wall $(INCLUDES)

OFILES =\
	obj/glad.o\
	obj/io.o\
	obj/render.o\
	obj/controls.o\
	obj/scene.o\
	obj/test_scene.o\
	obj/main.o

LIBS = -lGL -lglfw

BIN = bin/game

.PHONY: dirs clean purge

all: dirs $(OFILES)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OFILES) $(LIBS)

obj/glad.o: src/glad.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

obj/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

obj/%.o: src/%.cpp src/%.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

dirs:
	@mkdir -p bin/ obj/

clean:
	rm -rf obj/

purge: clean
	rm -rf bin/


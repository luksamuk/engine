OS?=linux

ifeq ($(OS),windows)
# Make sure you have GLFW and GLM for MinGW
	CXX = x86_64-w64-mingw32-g++
	LIBS = -lopengl32 -lglu32 -lglfw3 -lmingw32
	BIN = bin/$(OS)/game.exe
else
	CXX = g++
	LIBS = -lGL -lglfw -ltomlplusplus
	BIN = bin/$(OS)/game
endif

INCLUDES = -I./include
CXXFLAGS = --std=c++20 -g -Wall $(INCLUDES)

OFILES =\
	obj/$(OS)/glad.o\
	obj/$(OS)/io.o\
	obj/$(OS)/render.o\
	obj/$(OS)/controls.o\
	obj/$(OS)/scene.o\
	obj/$(OS)/test_scene.o\
	obj/$(OS)/sprite_scene.o\
	obj/$(OS)/movie_scene.o\
	obj/$(OS)/level_select.o\
	obj/$(OS)/core.o\
	obj/$(OS)/resources.o\
	obj/$(OS)/sprite.o\
	obj/$(OS)/tiled.o\
	obj/$(OS)/collision.o\
	obj/$(OS)/game_object.o\
	obj/$(OS)/quadtree.o\
	obj/$(OS)/partitioning.o\
	obj/$(OS)/partition_test.o\
	obj/$(OS)/main.o

.PHONY: dirs clean purge

all: dirs $(OFILES)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OFILES) $(LIBS)

obj/$(OS)/glad.o: src/glad.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

obj/$(OS)/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

obj/$(OS)/%.o: src/%.cpp src/%.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

dirs:
	@mkdir -p bin/$(OS) obj/$(OS)

clean:
	rm -rf obj/

purge: clean
	rm -f $(BIN)


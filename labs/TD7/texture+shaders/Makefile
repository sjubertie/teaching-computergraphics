
SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:src/%.cpp=obj/%.o)

BIN=bin/main

LDLIBS=-lGL -lGLU -lglut -lGLEW `pkg-config --libs opencv4`
CXXFLAGS=`pkg-config --cflags opencv4`

all: check $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

obj/%.o: src/%.cpp
	echo $(CXXFLAGS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

check:
	glslangValidator res/shaders/texture.vert.glsl
	glslangValidator res/shaders/texture.frag.glsl

clean:
	rm -rf bin/* obj/*

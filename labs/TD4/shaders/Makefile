
SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:src/%.cpp=obj/%.o)
BINS=$(OBJS:obj/%.o=bin/%)

SHADERS=$(wildcard res/shaders/*.glsl)

LDLIBS=-lGL -lGLU -lglut -lGLEW

all: $(BINS)

bin/%: obj/%.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

obj/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

obj/%.o: src/%.cpp include/%.hpp

check: $(SHADERS)
	@for shader in $^; do glslangValidator $$shader; done

clean:
	rm -f bin/* obj/*

.PHONY: all check run clean

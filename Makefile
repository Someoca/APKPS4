CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Iinclude
CORE = src/core/image.cpp src/graphics/pipeline.cpp src/hardware/profile.cpp src/library/library.cpp

all: build/gnm_scale

build/gnm_scale: $(CORE) src/main.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) $^ -o $@

build/test_pipeline: $(CORE) tests/test_pipeline.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) $^ -o $@

test: build/test_pipeline
	./build/test_pipeline

clean:
	rm -f build/gnm_scale build/test_pipeline
.PHONY: all test clean

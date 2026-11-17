CXX ?= g++
CPPFLAGS += -Iinclude -Isrc/moduloConcre
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic

ifeq ($(OS),Windows_NT)
SHELL := cmd.exe
EXE = .exe
else
EXE =
endif

MODULO = $(wildcard src/moduloConcre/*.cpp)
APP = bin/azimute$(EXE)
FUNCIONAL = bin/teste_funcional$(EXE)

build: | bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(wildcard src/*.cpp) $(MODULO) -o $(APP)

run: build
	./$(APP)

build_func: | bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(wildcard test/funcional/*.cpp) $(MODULO) -o $(FUNCIONAL)

run_func: build_func
	./$(FUNCIONAL)

all: build build_func

test: run_func

bin:
	mkdir bin

clean:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -NonInteractive -Command "foreach ($$arquivo in @('$(APP)', '$(FUNCIONAL)')) { if (Test-Path -LiteralPath $$arquivo) { Remove-Item -LiteralPath $$arquivo -Force -ErrorAction Stop } }"
else
	$(RM) $(APP) $(FUNCIONAL)
endif

.PHONY: all build run build_func run_func test clean

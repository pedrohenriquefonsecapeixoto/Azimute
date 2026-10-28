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
UNITARIO = bin/teste_unitario$(EXE)
FUNCIONAL = bin/teste_funcional$(EXE)

build: | bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(wildcard src/*.cpp) $(MODULO) -o $(APP)

run: build
	./$(APP)

build_unit: | bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(wildcard test/unit/*.cpp) $(MODULO) -o $(UNITARIO)

run_unit: build_unit
	./$(UNITARIO)

build_func: | bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(wildcard test/funcional/*.cpp) $(MODULO) -o $(FUNCIONAL)

run_func: build_func
	./$(FUNCIONAL)

all: build build_unit build_func

test: run_unit run_func

bin:
	mkdir bin

clean:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -NonInteractive -Command "foreach ($$arquivo in @('$(APP)', '$(UNITARIO)', '$(FUNCIONAL)')) { if (Test-Path -LiteralPath $$arquivo) { Remove-Item -LiteralPath $$arquivo -Force -ErrorAction Stop } }"
else
	$(RM) $(APP) $(UNITARIO) $(FUNCIONAL)
endif

.PHONY: all build run build_unit run_unit build_func run_func test clean

CXX           := clang++
CXXFLAGS      := -std=c++17 -Wall -Wextra -I /usr/lib/llvm-18/include/ -I lib/ -c -o
BINARY        := cap

BUILDDIR      := build
SRCDIR        := src
SOURCES       := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS       := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

.DEFAULT_GOAL := default
.PHONY: default prepare reset clean bin all run test help


default: all

prepare:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@printf "[\e[0;36mINFO\e[0m] Compiling object: %s\n" "$<"
	$(CXX) $(CXXFLAGS) $@ $< -MMD -MF ./$@.d

$(BINARY): $(OBJECTS)
	@printf "[\e[0;36mINFO\e[0m] Linking object: %s\n" "$@"
	$(CXX) -lclang-18 -I /usr/lib/llvm-18/include/ -I lib/ -o $@ $^

clean:
	@printf "[\e[0;36mINFO\e[0m] Cleaning up folder...\n"
	rm -f $(BINARY)
	rm -rf ./$(BUILDDIR)

bin: prepare $(BINARY)
	@printf "[\e[0;36mINFO\e[0m] Compiling binary...\n"
	chmod +x $(BINARY)

all: clean bin

-include $(wildcard $(BUILDDIR)/*.d)

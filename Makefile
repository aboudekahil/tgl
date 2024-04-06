CC=gcc
WCC=x86_64-w64-mingw32-gcc

FLAGS := -Wall -Wextra -Werror -std=c2x -D TGL_IMPLEMENTATION

COMPILER_INPUT := tgl.c -o ./build/tgl $(FLAGS)

all: linux

linux:
	$(CC) $(COMPILER_INPUT)

run:
	./build/tgl

debug:
	$(CC) -g $(COMPILER_INPUT)

windows:
	$(WCC) $(COMPILER_INPUT)

clean:
	rm build/*

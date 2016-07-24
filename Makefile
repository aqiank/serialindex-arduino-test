NAME=$(shell basename $(shell pwd))
CC=gcc
CFLAGS=-Wall `pkg-config --cflags sdl2 libserialport`
LDFLAGS=`pkg-config --libs sdl2 libserialport`

all: main.c
	$(CC) -o $(NAME) $? $(CFLAGS) $(LDFLAGS)

clean:
	-@rm $(NAME) 2> /dev/null || true

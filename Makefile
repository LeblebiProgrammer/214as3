CC = gcc
CFLAGS = -g -Wall

all: WTFserver

WTFserver: server.c
	$(CC) $(CFLAGS) -o WTFserver server.c helperFunctions.c




clean:
	rm -f WTFserver.o WTFserver

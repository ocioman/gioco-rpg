CC=gcc
CFLAGS=-std=c99 -pedantic -O2 -Wall

TARGET=GiocoRpgInfo

SRCS=GiocoRpgInfo.c

all: $(TARGET)

$(TARGET): $(SRCS) vector.h
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf html latex

run: all
	./$(TARGET)
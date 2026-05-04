# Simple Makefile for mbuf testing project

CC = cc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS =

# Object files
OBJS = main.o ieee80211.o athn_rxeof.o

# Target executable
TARGET = mbuf_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

main.o: main.c mbuf.h ieee80211.h
	$(CC) $(CFLAGS) -c main.c

athn_rxeof.o: athn_rxeof.c mbuf.h ieee80211.h endian.h
	$(CC) $(CFLAGS) -c athn_rxeof.c

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

CC = cc
CFLAGS = -Wall -Wextra -O2 -g -I.

OBJS = main.o ieee80211.o athn_rxeof.o mbuf.o

mbuf_test: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o mbuf_test

.PHONY: clean
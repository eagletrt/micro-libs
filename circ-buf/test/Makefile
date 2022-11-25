SHELL=/bin/bash
CC = gcc
CFLAGS = -g -Wall -std=c99 -DNOLOGGER

src_files = main.c ../circ-buf.c ../circ-buf.h ../../munit/munit.c ../../munit/munit.h tests.c tests.h

test: $(src_files)
	$(CC) $(CFLAGS) $(src_files) -o $@

clean:
	rm test

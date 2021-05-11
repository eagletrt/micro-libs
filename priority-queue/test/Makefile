SHELL=/bin/bash
CC = gcc
CFLAGS = -g -Wall -std=c99

src_files = main.c ../priority_queue.c ../priority_queue.h ../../munit/munit.c ../../munit/munit.h tests.c tests.h

test: $(src_files)
	$(CC) $(CFLAGS) $(src_files) -o $@

clean:
	rm test
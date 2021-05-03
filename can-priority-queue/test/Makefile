SHELL=/bin/bash
CC = gcc
CFLAGS = -g -DCANPQ_TEST -Wall -std=c99

src_files = main.c ../can_priority_queue.c ../can_priority_queue.h ../../munit/munit.c ../../munit/munit.h tests.c tests.h

test: $(src_files)
	$(CC) $(CFLAGS) $(src_files) -o $@

clean:
	rm test
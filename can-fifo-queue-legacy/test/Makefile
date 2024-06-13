SHELL=/bin/bash
CC = gcc
CFLAGS = -g -DCANFQ_TEST -Wall -std=c99

src_files = main.c ../can_fifo_queue.c ../can_fifo_queue.h ../../munit/munit.c ../../munit/munit.h tests.c tests.h

test: $(src_files)
	$(CC) $(CFLAGS) $(src_files) -o $@

clean:
	rm test
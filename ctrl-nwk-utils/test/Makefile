SHELL=/bin/bash
CC = gcc
CFLAGS = -g -Wall -std=c99

src_files = main.c ../ctrl-nwk-utils.c ../ctrl-nwk-utils.h ../../munit/munit.c ../../munit/munit.h tests.c tests.h

test: $(src_files)
	$(CC) $(CFLAGS) $(src_files) -o $@

clean:
	rm test

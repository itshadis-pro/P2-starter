# CS 313 P2 Makefile
#   make           build test_heap, test_student, and explore
#   make check     build and run the autograder's tests on your heap.c
#   make test      build and run your three tests against your heap.c
#   make run       build and run the optional comparison-count driver
#   make valgrind  run the tests under valgrind to check for leaks
#   make clean     remove built files

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g

.PHONY: all check test run valgrind clean

all: test_heap test_student explore

test_heap: test_heap.c heap.c heap.h
	$(CC) $(CFLAGS) -o test_heap test_heap.c heap.c

test_student: test_student.c heap.c heap.h check.h
	$(CC) $(CFLAGS) -o test_student test_student.c heap.c

explore: explore.c heap.c heap.h
	$(CC) $(CFLAGS) -O2 -o explore explore.c heap.c

check: test_heap
	./test_heap

test: test_student
	./test_student

run: explore
	./explore

valgrind: test_heap
	valgrind --leak-check=full --error-exitcode=1 ./test_heap valgrind_scenario

clean:
	rm -rf test_heap test_student explore *.dSYM

/*
 * test_student.c -- CS 313 P2: your three hand-traced tests.
 *
 * Name:
 *
 * Write three tests. Each one builds a heap, then checks the heap array
 * with CHECK_HEAP(h, k0, k1, ...), where the keys are an array YOU worked
 * out by hand before running anything. Show your trace in a comment
 * above the check (the array after each step). You may add CHECK(...)
 * lines for anything else, such as the value extract_max returns.
 *
 * The autograder runs your tests against the instructor's heap. A test
 * passes if all its checks pass there and it has at least one passing
 * CHECK_HEAP with 5 or more keys. So a wrong hand trace fails the test.
 *
 * Use 0-indexed positions: parent (i-1)/2, children 2i+1 and 2i+2.
 *
 * Suggested tests:
 *   1. Insert at least 7 keys, in an order that makes some keys sift up
 *      more than one level.
 *   2. heap_build on an array of at least 7 keys, then check the array.
 *   3. Insert or build, then call heap_extract_max two or three times,
 *      and check the array after the last extract.
 *
 * Worked example (not one of your three):
 *
 *   Heap *h = heap_create(10);
 *   heap_insert(h, 3);    // [3]
 *   heap_insert(h, 8);    // [3, 8] -> 8 > parent 3, swap -> [8, 3]
 *   heap_insert(h, 5);    // [8, 3, 5], 5 < parent 8, stop
 *   heap_insert(h, 9);    // [8, 3, 5, 9] -> swap with 3 -> [8, 9, 5, 3]
 *                         //   -> swap with 8 -> [9, 8, 5, 3]
 *   heap_insert(h, 1);    // [9, 8, 5, 3, 1], 1 < parent 8, stop
 *   CHECK_HEAP(h, 9, 8, 5, 3, 1);
 *   heap_destroy(h);
 *
 * Build and run against your own heap.c:  make test
 */
#include "check.h"

void student_test_1(void) {
    Heap *h = heap_create(20);
    /* TODO */
    heap_destroy(h);
}

void student_test_2(void) {
    /* TODO: replace these keys with your own. */
    int a[] = {0};
    Heap *h = heap_build(a, 0, 20);
    /* TODO */
    heap_destroy(h);
}

void student_test_3(void) {
    Heap *h = heap_create(20);
    /* TODO */
    heap_destroy(h);
}

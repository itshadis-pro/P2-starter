/*
 * test_student.c -- CS 313 P2: your three hand-traced tests.
 *
 * Name: Hadi Almarzooq
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

    /* Trace:
       Insert 1: [1]
       Insert 2: [2, 1]
       Insert 3: [3, 1, 2]
       Insert 4: [4, 3, 2, 1]
       Insert 5: [5, 4, 2, 1, 3]
       Insert 6: [6, 4, 5, 1, 3, 2]
       Insert 7: [7, 4, 6, 1, 3, 2, 5]*/

    heap_insert(h, 1);
    heap_insert(h, 2);
    heap_insert(h, 3);
    heap_insert(h, 4);
    heap_insert(h, 5);
    heap_insert(h, 6);
    heap_insert(h, 7);    heap_destroy(h);
}

void student_test_2(void) {
    /* TODO: replace these keys with your own. */
    int a[] = {1, 2, 3, 4, 5, 6, 7};
    Heap *h = heap_build(a, 7, 20);
    /* TODO */
    CHECK_HEAP(h, 7, 5, 6, 4, 2, 1, 3);

    heap_destroy(h);
}

void student_test_3(void) {

    /* Trace extract_max twice on built heap [7, 5, 6, 4, 2, 1, 3]:
       Extract 1: root 7 removed, 3 moves to root -> [3, 5, 6, 4, 2, 1]
                  sift 3 down: swap with 6 -> [6, 5, 3, 4, 2, 1]
       Extract 2: root 6 removed, 1 moves to root -> [1, 5, 3, 4, 2]
                  sift 1 down: swap with 5 -> [5, 1, 3, 4, 2]
                               swap with 4 -> [5, 4, 3, 1, 2]
                               
                               Used Gemini to help generating test cases.
    */
    int a[] = {1, 2, 3, 4, 5, 6, 7};
    Heap *h = heap_build(a, 7, 20);
    int out;

    heap_extract_max(h, &out);
    heap_extract_max(h, &out);

    CHECK_HEAP(h, 5, 4, 3, 1, 2);
    
    heap_destroy(h);
}
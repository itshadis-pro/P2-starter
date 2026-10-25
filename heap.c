/*
 * heap.c -- CS 313 P2: a binary max-heap of ints.
 *
 * Name:
 *
 * Fill in every TODO. The rules are in heap.h and README.md.
 * This skeleton compiles, but it fails the tests until you finish it.
 * Write sift_up and sift_down with loops. You do not need recursion.
 */
#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

/* ---- Helpers (provided) ---------------------------------------------- */
/* HELPER means "static": the function is private to this file. It also
 * stops the compiler from warning about a helper you have not used yet. */
#define HELPER static __attribute__((unused))

HELPER int parent(int i) { return (i - 1) / 2; }
HELPER int left(int i)   { return 2 * i + 1; }
HELPER int right(int i)  { return 2 * i + 2; }

HELPER void swap(int *a, int i, int j) {
    int t = a[i];
    a[i] = a[j];
    a[j] = t;
}

/* Return x > y. Adds 1 to *comparisons unless comparisons is NULL.
 * Use this for every key comparison so the counter stays correct. */
HELPER bool greater(int x, int y, long *comparisons) {
    if (comparisons != NULL) {
        (*comparisons)++;
    }
    return x > y;
}

/* ---- Helpers you write ------------------------------------------------ */
/* Each helper works on a plain int array, so heap_sort can use it too.
 * Pass &h->comparisons as the last argument from the heap functions,
 * and NULL from heap_sort. */

/* Move a[i] up while it is strictly greater than its parent. */
HELPER void sift_up(int *a, int i, long *comparisons) {
    /* TODO */
    (void)a;
    (void)i;
    (void)comparisons;
}

/* a[0 .. n-1] is the heap. Move a[i] down: find the largest of a[i] and
 * its children that exist (index < n), using strict > (left first, then
 * right). If that is a child, swap and continue from the child. */
HELPER void sift_down(int *a, int n, int i, long *comparisons) {
    /* TODO */
    (void)a;
    (void)n;
    (void)i;
    (void)comparisons;
}

/* Rearrange a[0 .. n-1] into a max-heap: sift down each index from
 * n/2 - 1 down to 0 (BUILD-MAX-HEAP). */
HELPER void build(int *a, int n, long *comparisons) {
    /* TODO */
    (void)a;
    (void)n;
    (void)comparisons;
}
/* ---- Public functions (see heap.h) ------------------------------------ */

Heap *heap_create(int capacity) {
    if (capacity < 1) {
        return NULL;
    }

    // 1. Allocate the Heap struct
    Heap *h = malloc(sizeof(Heap));
    if (h == NULL) {
        return NULL;
    }

    // 2. Allocate the data array for 'capacity' integers
    h->data = malloc(data* sizeof(capacity));
    
    if (h->data == NULL) {
        // Free the struct we successfully allocated above
        free(h); 
        return NULL;
    }

    // 3. Initialize the fields
    h->capacity = capacity;
    h->size = 0;
    h->comparisons = 0;

    return h;
}

void heap_destroy(Heap *h) {
    /* if h is not NULL, free the data array, then free h. */
    if (h != NULL){
        free(h->data);
        free(h);
    }
}

bool heap_insert(Heap *h, int key) {
    /* TODO: return false if full. Otherwise store key at data[size],
     * grow size, sift the new key up, and return true. */
    (void)h;
    (void)key;
    return false;
}

bool heap_peek(const Heap *h, int *out) {
    /* TODO */
    (void)h;
    (void)out;
    return false;
}

bool heap_extract_max(Heap *h, int *out) {
    /* TODO: return false if empty. Otherwise save the root in *out,
     * move the LAST key to the root, shrink size, sift the root down
     * within the new size, and return true. */
    (void)h;
    (void)out;
    return false;
}

Heap *heap_build(const int *a, int n, int capacity) {
    /* TODO: check the arguments, create a heap, copy a into data,
     * set size, then build. */
    (void)a;
    (void)n;
    (void)capacity;
    return NULL;
}

void heap_sort(int *a, int n) {
    /* TODO: build a max-heap in a. Then for end = n-1 down to 1: swap
     * a[0] and a[end], and sift a[0] down within a[0 .. end-1]. */
    (void)a;
    (void)n;
}

int heap_size(const Heap *h) {
    /* TODO */
    if (h == NULL){
        return 0;
    }
    return h->size;
}

int heap_to_array(const Heap *h, int *out) {
    /* TODO: copy data[0 .. size-1] into out and return size. */

    if (h == NULL){
        return 0;
    }

    // Loop through the array exactly 'size' times
    for (int i = 0; i < h->size; i++){
        // copy each element form the heap's data array into the 'out' array
        out[i] = h->data[i];
    }
    return h->size;
}

long heap_comparisons(const Heap *h) {
    /* TODO */
    (void)h;
    return -1;
}

/* ---- Debug printing (provided) ---------------------------------------- */

void heap_print(const Heap *h) {
    if (h == NULL) {
        printf("(null heap)\n");
        return;
    }
    printf("size %d, capacity %d: [", h->size, h->capacity);
    for (int i = 0; i < h->size; i++) {
        printf(i == 0 ? "%d" : ", %d", h->data[i]);
    }
    printf("]\n");
    /* Level k holds indices 2^k - 1 .. 2^(k+1) - 2. */
    for (int start = 0, level = 0; start < h->size;
         start = 2 * start + 1, level++) {
        printf("  level %d:", level);
        for (int i = start; i < h->size && i <= 2 * start; i++) {
            printf(" %d", h->data[i]);
        }
        printf("\n");
    }
}

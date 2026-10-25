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

/* Move a[i] up while it is strictly greater than its parent. */HELPER void sift_up(int *a, int i, long *comparisons) {
    while (i > 0 && greater(a[i], a[parent(i)], comparisons)) {
        swap(a, i, parent(i));
        i = parent(i);
    }
}

HELPER void sift_down(int *a, int n, int i, long *comparisons) {
    while (left(i) < n) {
        int l = left(i);
        int r = right(i);
        int largest = i;

        if (greater(a[l], a[largest], comparisons)) {
            largest = l;
        }
        
        if (r < n && greater(a[r], a[largest], comparisons)) {
            largest = r;
        }

        if (largest != i) {
            swap(a, i, largest);
            i = largest;
        } else {
            break;
        }
    }
}

HELPER void build(int *a, int n, long *comparisons) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        sift_down(a, n, i, comparisons);
    }
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
    h->data = malloc(capacity * sizeof(int));   

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
    if (h->size == h->capacity) {
        return false;
    }
    h->data[h->size] = key;
    h->size = h->size + 1;
    sift_up(h->data, h->size - 1, &h->comparisons);
    return true;
}

bool heap_peek(const Heap *h, int *out) {
    if (h->size == 0) {
        return false;
    }
    *out = h->data[0];
    return true;
}

bool heap_extract_max(Heap *h, int *out) {
    if (h->size == 0) {
        return false;
    }
    *out = h->data[0];
    h->data[0] = h->data[h->size - 1];
    h->size = h->size - 1;
    sift_down(h->data, h->size, 0, &h->comparisons);
    return true;
}

Heap *heap_build(const int *a, int n, int capacity) {
    if (capacity < 1 || n < 0 || n > capacity || (n > 0 && a == NULL)) {
        return NULL;
    }
    Heap *h = heap_create(capacity);
    if (h == NULL) {
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        h->data[i] = a[i];
    }
    h->size = n;
    build(h->data, h->size, &h->comparisons);
    return h;
}

void heap_sort(int *a, int n) {
    if (a == NULL || n <= 1) {
        return;
    }
    build(a, n, NULL);
    for (int end = n - 1; end > 0; end--) {
        swap(a, 0, end);
        sift_down(a, end, 0, NULL);
    }
}

int heap_size(const Heap *h) {
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
    if (h == NULL){
        return 0;
    }
    return h->comparisons;
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

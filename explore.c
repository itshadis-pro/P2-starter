/*
 * explore.c -- CS 313 P2 optional exploration driver. Do not edit.
 * Not graded.
 *
 * For each n, make a heap from the same n keys two ways:
 *   (a) n calls to heap_insert on an empty heap,
 *   (b) one call to heap_build,
 * and print heap_comparisons for each, divided by n.
 * It does this for keys in random order and in increasing order.
 * Your counts are only right if every key comparison uses greater().
 */
#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

static void run(const char *order, const int *keys, int n) {
    Heap *a = heap_create(n);
    if (a == NULL) {
        printf("heap_create returned NULL\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        heap_insert(a, keys[i]);
    }
    Heap *b = heap_build(keys, n, n);
    if (b == NULL) {
        printf("heap_build returned NULL\n");
        exit(1);
    }
    long ca = heap_comparisons(a), cb = heap_comparisons(b);
    printf("%-11s %8d %14ld %8.2f %14ld %8.2f\n", order, n,
           ca, (double)ca / n, cb, (double)cb / n);
    heap_destroy(a);
    heap_destroy(b);
}

int main(void) {
    int ns[] = {1000, 10000, 100000, 1000000};
    printf("%-11s %8s %14s %8s %14s %8s\n", "order", "n",
           "insert cmps", "per key", "build cmps", "per key");
    for (int k = 0; k < 4; k++) {
        int n = ns[k];
        int *keys = malloc(n * sizeof(int));
        srand(313);
        for (int i = 0; i < n; i++) {
            keys[i] = rand();
        }
        run("random", keys, n);
        for (int i = 0; i < n; i++) {
            keys[i] = i;
        }
        run("increasing", keys, n);
        free(keys);
    }
    return 0;
}

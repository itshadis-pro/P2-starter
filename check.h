/*
 * check.h -- tiny test framework for test_student.c. Do not edit.
 * The autograder uses its own copy.
 *
 *   CHECK(condition)          fails the test if condition is false.
 *   CHECK_HEAP(h, k0, k1, ...)
 *                             fails the test unless the heap array is
 *                             exactly k0, k1, ... (data[0], data[1], ...,
 *                             data[size-1]). The keys must be integer
 *                             literals you worked out by hand, such as
 *                             CHECK_HEAP(h, 9, 5, 8, 1, 3)
 *                             (not variables or expressions).
 *
 * A test passes if every check passes AND it contains at least one
 * passing CHECK_HEAP with 5 or more keys.
 *
 * Run all three tests:  ./test_student
 * Run one test:         ./test_student 2
 */
#ifndef CHECK_H
#define CHECK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "heap.h"

#define CHECK_HEAP_MIN_KEYS 5

static int check_count = 0;
static int check_failures = 0;
static int check_heap_passed = 0;

#define CHECK(cond)                                                     \
    do {                                                                \
        check_count++;                                                  \
        if (!(cond)) {                                                  \
            printf("    FAIL line %d: CHECK(%s)\n", __LINE__, #cond);   \
            check_failures++;                                           \
        }                                                               \
    } while (0)

#define CHECK_HEAP(h, ...)                                              \
    check_heap_impl((h), (const int[]){__VA_ARGS__},                    \
                    (int)(sizeof((const int[]){__VA_ARGS__})            \
                          / sizeof(int)),                               \
                    #__VA_ARGS__, __LINE__)

static inline void check_print(const char *label, const int *a, int n) {
    printf("        %s [", label);
    for (int i = 0; i < n; i++) {
        printf(i == 0 ? "%d" : ", %d", a[i]);
    }
    printf("]\n");
}

static inline void check_heap_impl(const Heap *h, const int *expected,
                                   int n, const char *text, int line) {
    check_count++;
    char prev = ',';  /* last non-space character seen */
    for (const char *p = text; *p != '\0'; p++) {
        int ok = isdigit((unsigned char)*p) || *p == ','
                 || isspace((unsigned char)*p)
                 || (*p == '-' && prev == ',');  /* a minus sign */
        if (!isspace((unsigned char)*p)) {
            prev = *p;
        }
        if (!ok) {
            printf("    FAIL line %d: CHECK_HEAP needs integer literals"
                   " you worked out by hand, not \"%s\"\n", line, text);
            check_failures++;
            return;
        }
    }
    if (h == NULL) {
        printf("    FAIL line %d: heap is NULL\n", line);
        check_failures++;
        return;
    }
    int size = heap_size(h);
    int *actual = malloc((size > 0 ? size : 1) * sizeof(int));
    int count = heap_to_array(h, actual);
    if (size != n || count != n || memcmp(actual, expected,
                                          n * sizeof(int)) != 0) {
        printf("    FAIL line %d: the heap array is not what you"
               " expected\n", line);
        check_print("you expected", expected, n);
        check_print("the heap has", actual, count);
        check_failures++;
    } else if (n >= CHECK_HEAP_MIN_KEYS) {
        check_heap_passed++;
    }
    free(actual);
}

void student_test_1(void);
void student_test_2(void);
void student_test_3(void);

int main(int argc, char **argv) {
    void (*tests[3])(void) = {student_test_1, student_test_2, student_test_3};
    int first = 1, last = 3, all_pass = 1;

    if (argc > 1) {
        first = last = atoi(argv[1]);
        if (first < 1 || first > 3) {
            printf("usage: %s [1|2|3]\n", argv[0]);
            return 2;
        }
    }
    for (int t = first; t <= last; t++) {
        check_count = check_failures = check_heap_passed = 0;
        printf("student_test_%d:\n", t);
        tests[t - 1]();
        int pass = check_failures == 0 && check_heap_passed >= 1;
        if (check_failures == 0 && check_heap_passed == 0) {
            printf("    FAIL: no passing CHECK_HEAP with %d or more keys\n",
                   CHECK_HEAP_MIN_KEYS);
        }
        printf("    %s (%d checks)\n", pass ? "PASS" : "FAIL", check_count);
        fflush(stdout);
        all_pass = all_pass && pass;
    }
    return all_pass ? 0 : 1;
}

#endif

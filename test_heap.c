/*
 * test_heap.c -- CS 313 P2 tests. Do not edit.
 *
 * These are the same tests the Gradescope autograder runs (it also runs
 * your three tests from test_student.c and a valgrind leak check).
 *
 *   ./test_heap          run every test, each in its own process
 *   ./test_heap NAME     run one test, e.g. ./test_heap insert_order
 *   ./test_heap --list   list the tests
 *
 * When run all together, each test has a 5-second limit, so an infinite
 * loop fails cleanly. A single test prints "passed" when it passes.
 */
#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "heap.h"

#define TIME_LIMIT 5

/* ---- Tiny assertion helpers ------------------------------------------ */

static int failed;

#define FAIL(...)                                                       \
    do {                                                                \
        printf("    ");                                                 \
        printf(__VA_ARGS__);                                            \
        printf("\n");                                                   \
        failed = 1;                                                     \
        return;                                                         \
    } while (0)

#define REQUIRE(cond, ...)                                              \
    do {                                                                \
        if (!(cond)) {                                                  \
            FAIL(__VA_ARGS__);                                          \
        }                                                               \
    } while (0)

#define LEN(arr) ((int)(sizeof(arr) / sizeof((arr)[0])))

static void print_array(const char *label, const int *a, int n) {
    printf("    %s [", label);
    for (int i = 0; i < n; i++) {
        printf(i == 0 ? "%d" : ", %d", a[i]);
    }
    printf("]\n");
}

static int same_array(const int *a, const int *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

/* Compare the heap's array with exp[0 .. n-1]. Print both on mismatch. */
static int heap_matches(const Heap *h, const int *exp, int n,
                        const char *what) {
    int got[1024];
    int size = heap_size(h);
    if (size != n) {
        printf("    %s: heap_size is %d, expected %d\n", what, size, n);
        return 0;
    }
    int count = heap_to_array(h, got);
    if (count != n) {
        printf("    %s: heap_to_array returned %d, expected %d\n",
               what, count, n);
        return 0;
    }
    if (!same_array(got, exp, n)) {
        printf("    %s: wrong heap array\n", what);
        print_array("expected", exp, n);
        print_array("got     ", got, n);
        return 0;
    }
    return 1;
}

#define REQUIRE_HEAP(h, exp, n, what)                                   \
    do {                                                                \
        if (!heap_matches((h), (exp), (n), (what))) {                   \
            failed = 1;                                                 \
            return;                                                     \
        }                                                               \
    } while (0)

static int is_max_heap(const int *a, int n) {
    for (int i = 1; i < n; i++) {
        if (a[(i - 1) / 2] < a[i]) {
            return 0;
        }
    }
    return 1;
}

static int cmp_int(const void *x, const void *y) {
    int a = *(const int *)x, b = *(const int *)y;
    return (a > b) - (a < b);
}

/* Create a heap and insert keys; fail the test on any problem. */
#define MAKE_HEAP(h, cap, keys)                                         \
    Heap *h = heap_create(cap);                                         \
    REQUIRE(h != NULL, "heap_create(%d) returned NULL", (cap));         \
    for (int k_ = 0; k_ < LEN(keys); k_++) {                            \
        if (!heap_insert(h, (keys)[k_])) {                              \
            heap_destroy(h);                                            \
            FAIL("heap_insert(%d) returned false on a heap that is not"  \
                 " full", (keys)[k_]);                                  \
        }                                                               \
    }

/* The 24F lab's standard sequence and its heap after inserts. */
static const int SEQ[] = {10, 24, 3, 57, 4, 67, 37, 87, 7};
static const int SEQ_INSERTED[] = {87, 67, 57, 24, 4, 3, 37, 10, 7};
static const int SEQ_BUILT[] = {87, 57, 67, 24, 4, 3, 37, 10, 7};

/* ---- heap_sort checker with guard cells ------------------------------ */
/* The array to sort sits between guard cells holding INT_MAX. A sort that
 * reads or writes one cell past either end moves a guard value. */

#define GUARD 2

static int sort_ok(const int *in, int n, const char *what) {
    int *buf = malloc((n + 2 * GUARD) * sizeof(int));
    int *expect = malloc((n + 1) * sizeof(int));
    int ok = 1;
    for (int i = 0; i < GUARD; i++) {
        buf[i] = INT_MAX;
        buf[n + GUARD + i] = INT_MAX;
    }
    memcpy(buf + GUARD, in, n * sizeof(int));
    memcpy(expect, in, n * sizeof(int));
    qsort(expect, n, sizeof(int), cmp_int);
    heap_sort(buf + GUARD, n);
    for (int i = 0; i < GUARD; i++) {
        if (buf[i] != INT_MAX || buf[n + GUARD + i] != INT_MAX) {
            printf("    %s (n = %d): heap_sort changed memory outside"
                   " a[0 .. n-1] (check your index bounds)\n", what, n);
            ok = 0;
            break;
        }
    }
    if (ok && !same_array(buf + GUARD, expect, n)) {
        printf("    %s (n = %d): not sorted correctly\n", what, n);
        if (n <= 30) {
            print_array("input   ", in, n);
            print_array("expected", expect, n);
            print_array("got     ", buf + GUARD, n);
        }
        ok = 0;
    }
    free(buf);
    free(expect);
    return ok;
}

#define REQUIRE_SORT(in, n, what)                                       \
    do {                                                                \
        if (!sort_ok((in), (n), (what))) {                              \
            failed = 1;                                                 \
            return;                                                     \
        }                                                               \
    } while (0)

/* ======================================================================
 * Tests
 * ==================================================================== */

static void test_empty_contract(void) {
    Heap *h = heap_create(10);
    REQUIRE(h != NULL, "heap_create(10) returned NULL");
    int out = -999, buf[1];
    int size = heap_size(h);
    int pk = heap_peek(h, &out);
    int pk_out = out;
    int ex = heap_extract_max(h, &out);
    int ex_out = out;
    int count = heap_to_array(h, buf);
    heap_destroy(h);
    REQUIRE(size == 0, "new heap: heap_size is %d, expected 0", size);
    REQUIRE(!pk, "heap_peek on an empty heap returned true");
    REQUIRE(pk_out == -999, "heap_peek on an empty heap changed *out");
    REQUIRE(!ex, "heap_extract_max on an empty heap returned true");
    REQUIRE(ex_out == -999,
            "heap_extract_max on an empty heap changed *out");
    REQUIRE(count == 0, "heap_to_array on an empty heap returned %d",
            count);
}

static void test_insert_order(void) {
    MAKE_HEAP(h, 10, SEQ);
    printf("    inserted 10, 24, 3, 57, 4, 67, 37, 87, 7\n");
    int ok = heap_matches(h, SEQ_INSERTED, 9, "after 9 inserts");
    heap_destroy(h);
    REQUIRE(ok, "(sift-up: parent of i is (i-1)/2)");
}

static void test_peek(void) {
    MAKE_HEAP(h, 10, SEQ);
    int out = -1;
    int pk = heap_peek(h, &out);
    int ok = heap_matches(h, SEQ_INSERTED, 9, "after heap_peek");
    heap_destroy(h);
    REQUIRE(pk, "heap_peek on a nonempty heap returned false");
    REQUIRE(out == 87, "heap_peek gave %d, expected 87", out);
    REQUIRE(ok, "heap_peek must not change the heap");
}

static void test_extract_order(void) {
    const int expect[] = {87, 67, 57, 37, 24, 10, 7, 4, 3};
    MAKE_HEAP(h, 10, SEQ);
    for (int i = 0; i < 9; i++) {
        int out = -1;
        if (!heap_extract_max(h, &out)) {
            heap_destroy(h);
            FAIL("extract %d of 9 returned false", i + 1);
        }
        if (out != expect[i] || heap_size(h) != 8 - i) {
            int size = heap_size(h);
            heap_destroy(h);
            FAIL("extract %d of 9 gave %d with size %d after; expected"
                 " %d with size %d", i + 1, out, size, expect[i], 8 - i);
        }
    }
    int out = -999;
    int ex = heap_extract_max(h, &out);
    heap_destroy(h);
    REQUIRE(!ex, "extract from the emptied heap returned true");
    REQUIRE(out == -999, "extract from the emptied heap changed *out");
}

static void test_insert_full(void) {
    const int keys[] = {1, 2, 3, 4, 5};
    const int expect[] = {5, 4, 2, 1, 3};
    MAKE_HEAP(h, 5, keys);
    int ins = heap_insert(h, 6);
    int ok = heap_matches(h, expect, 5, "capacity 5, after inserting"
                          " 1..5 and then 6");
    heap_destroy(h);
    REQUIRE(!ins, "heap_insert on a full heap returned true");
    REQUIRE(ok, "a failed heap_insert must leave the heap unchanged");
}

static void test_build_heap(void) {
    int a[9];
    memcpy(a, SEQ, sizeof(a));
    Heap *h = heap_build(a, 9, 9);
    REQUIRE(h != NULL, "heap_build(a, 9, 9) returned NULL");
    printf("    heap_build on [10, 24, 3, 57, 4, 67, 37, 87, 7]\n");
    int ok = heap_matches(h, SEQ_BUILT, 9, "after heap_build");
    heap_destroy(h);
    REQUIRE(ok, "(build: sift down i = n/2-1 down to 0; left child wins"
            " ties)");
    REQUIRE(same_array(a, SEQ, 9), "heap_build changed its input array");
}

static void test_heap_sort(void) {
    const int a[] = {10, 24, 3, 57, 4, 67, 37, 87, 7};
    const int b[] = {85, 34, 35, 17, 33, 31, 14, 42, 41, 19, 77,
                     23, 78, 60, 13, 71, 2, 43, 57, 4};
    REQUIRE_SORT(a, LEN(a), "9 keys");
    REQUIRE_SORT(b, LEN(b), "20 keys");
}

/* ======================================================================
 * Other tests
 * ==================================================================== */

static void test_create_capacity(void) {
    Heap *z = heap_create(0);
    Heap *neg = heap_create(-3);
    heap_destroy(z);
    heap_destroy(neg);
    REQUIRE(z == NULL, "heap_create(0) should return NULL");
    REQUIRE(neg == NULL, "heap_create(-3) should return NULL");
    heap_destroy(NULL);
    Heap *h = heap_create(1);
    REQUIRE(h != NULL, "heap_create(1) returned NULL");
    int a = heap_insert(h, 5);
    int b = heap_insert(h, 6);
    int out = -1;
    int c = heap_peek(h, &out);
    heap_destroy(h);
    REQUIRE(a, "capacity 1: first insert returned false");
    REQUIRE(!b, "capacity 1: second insert returned true");
    REQUIRE(c && out == 5, "capacity 1: peek should give 5");
}

static void test_extract_layout(void) {
    const int keys[] = {1, 2, 3};
    const int before[] = {3, 1, 2};
    const int after[] = {2, 1};
    MAKE_HEAP(h, 5, keys);
    if (!heap_matches(h, before, 3, "after inserting 1, 2, 3")) {
        heap_destroy(h);
        FAIL("(see above)");
    }
    int out = -1;
    int ex = heap_extract_max(h, &out);
    int ok = heap_matches(h, after, 2, "after one extract");
    heap_destroy(h);
    REQUIRE(ex && out == 3, "extract gave %d, expected 3", out);
    REQUIRE(ok, "(move the LAST key to the root, then sift down)");
}

static void test_mixed_ops(void) {
    const int keys[] = {1, 2, 3};
    const int rest[] = {2, 1};
    MAKE_HEAP(h, 5, keys);
    int o1 = -1, o2 = -1, o3 = -1;
    heap_extract_max(h, &o1);
    int i4 = heap_insert(h, 4);
    int i5 = heap_insert(h, 5);
    heap_extract_max(h, &o2);
    heap_extract_max(h, &o3);
    int ok = heap_matches(h, rest, 2, "at the end");
    heap_destroy(h);
    REQUIRE(i4 && i5, "an insert returned false");
    REQUIRE(o1 == 3 && o2 == 5 && o3 == 4,
            "insert 1,2,3; extract; insert 4,5; extract; extract gave"
            " %d, %d, %d, expected 3, 5, 4", o1, o2, o3);
    REQUIRE(ok, "(see above)");
}

static void test_extract_six(void) {
    const int expect[] = {7, 4, 3};
    MAKE_HEAP(h, 10, SEQ);
    for (int i = 0; i < 6; i++) {
        int out;
        heap_extract_max(h, &out);
    }
    int ok = heap_matches(h, expect, 3,
                          "24F sequence, after 6 extracts");
    heap_destroy(h);
    REQUIRE(ok, "(see above)");
}

static void test_empty_then_reuse(void) {
    const int keys[] = {10, 24, 3};
    MAKE_HEAP(h, 10, keys);
    int o[3] = {-1, -1, -1};
    for (int i = 0; i < 3; i++) {
        heap_extract_max(h, &o[i]);
    }
    int out = -999;
    int pk = heap_peek(h, &out);
    int ins = heap_insert(h, 8);
    int out2 = -1;
    int pk2 = heap_peek(h, &out2);
    heap_destroy(h);
    REQUIRE(o[0] == 24 && o[1] == 10 && o[2] == 3,
            "extracts gave %d, %d, %d, expected 24, 10, 3",
            o[0], o[1], o[2]);
    REQUIRE(!pk && out == -999,
            "heap_peek after emptying should return false and leave"
            " *out alone");
    REQUIRE(ins && pk2 && out2 == 8,
            "after emptying, insert 8 then peek should give 8");
}

static void test_build_idempotent(void) {
    int a[9];
    memcpy(a, SEQ, sizeof(a));
    for (int round = 1; round <= 5; round++) {
        Heap *h = heap_build(a, 9, 9);
        REQUIRE(h != NULL, "heap_build returned NULL");
        int ok = heap_matches(h, SEQ_BUILT, 9, "repeated heap_build");
        heap_to_array(h, a);
        heap_destroy(h);
        REQUIRE(ok, "(round %d: building from a heap must not change it)",
                round);
    }
}

static void test_build_small(void) {
    const int one[] = {10};
    const int two[] = {1, 2};
    const int two_heap[] = {2, 1};
    const int seven[] = {7};
    Heap *h = heap_build(one, 1, 1);
    REQUIRE(h != NULL, "heap_build(one key) returned NULL");
    int ok = heap_matches(h, one, 1, "build on [10]");
    heap_destroy(h);
    REQUIRE(ok, "(see above)");

    h = heap_build(two, 2, 2);
    REQUIRE(h != NULL, "heap_build(two keys) returned NULL");
    ok = heap_matches(h, two_heap, 2, "build on [1, 2]");
    heap_destroy(h);
    REQUIRE(ok, "(see above)");

    h = heap_build(NULL, 0, 4);
    REQUIRE(h != NULL, "heap_build(NULL, 0, 4) returned NULL");
    int out = -999;
    int pk = heap_peek(h, &out);
    int ins = heap_insert(h, 7);
    ok = heap_matches(h, seven, 1, "build with n = 0, then insert 7");
    heap_destroy(h);
    REQUIRE(!pk && out == -999, "peek on a heap built from 0 keys");
    REQUIRE(ins && ok, "(see above)");
}

static void test_build_args(void) {
    const int a[] = {3, 1, 2};
    Heap *x = heap_build(a, 3, 2);
    Heap *y = heap_build(a, -1, 5);
    Heap *z = heap_build(a, 0, 0);
    heap_destroy(x);
    heap_destroy(y);
    heap_destroy(z);
    REQUIRE(x == NULL, "heap_build with capacity < n should return NULL");
    REQUIRE(y == NULL, "heap_build with n < 0 should return NULL");
    REQUIRE(z == NULL, "heap_build with capacity 0 should return NULL");

    const int expect[] = {10, 9, 2, 1, 3};
    Heap *h = heap_build(a, 3, 5);
    REQUIRE(h != NULL, "heap_build(a, 3, 5) returned NULL");
    int i9 = heap_insert(h, 9);
    int i10 = heap_insert(h, 10);
    int i11 = heap_insert(h, 11);
    int ok = heap_matches(h, expect, 5,
                          "build [3, 1, 2] with capacity 5, insert 9, 10");
    heap_destroy(h);
    REQUIRE(i9 && i10, "insert into a built heap with room returned"
            " false");
    REQUIRE(!i11, "insert into a full built heap returned true");
    REQUIRE(ok, "(see above)");
}

static void test_ties(void) {
    /* Equal children: the LEFT child wins. */
    const int a[] = {1, 5, 5, 4, 0, 0, 0};
    const int a_heap[] = {5, 4, 5, 1, 0, 0, 0};
    const int b[] = {2, 7, 7, 3, 3};
    const int b_heap[] = {7, 3, 7, 2, 3};
    const int b_after[] = {7, 3, 3, 2};
    Heap *h = heap_build(a, 7, 7);
    REQUIRE(h != NULL, "heap_build returned NULL");
    int ok = heap_matches(h, a_heap, 7, "build on [1, 5, 5, 4, 0, 0, 0]");
    heap_destroy(h);
    REQUIRE(ok, "(equal children: sift down to the LEFT child)");

    h = heap_build(b, 5, 5);
    REQUIRE(h != NULL, "heap_build returned NULL");
    ok = heap_matches(h, b_heap, 5, "build on [2, 7, 7, 3, 3]");
    int out = -1;
    heap_extract_max(h, &out);
    int ok2 = ok && heap_matches(h, b_after, 4, "then extract once");
    heap_destroy(h);
    REQUIRE(ok, "(equal children: sift down to the LEFT child)");
    REQUIRE(out == 7 && ok2, "(see above)");
}

static void test_sort_edge(void) {
    const int two_up[] = {1, 2};
    const int two_down[] = {2, 1};
    const int up[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const int down[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    const int same[] = {7, 7, 7, 7, 7, 7};
    const int dups[] = {3, 1, 3, 2, 1, 3, 2, 1};
    const int neg[] = {0, -5, INT_MIN, 12, -1, INT_MAX - 1, -5, 3};
    REQUIRE_SORT(up, 0, "empty array");
    REQUIRE_SORT(up, 1, "one key");
    REQUIRE_SORT(two_up, 2, "[1, 2]");
    REQUIRE_SORT(two_down, 2, "[2, 1]");
    REQUIRE_SORT(up, LEN(up), "already sorted");
    REQUIRE_SORT(down, LEN(down), "reverse sorted");
    REQUIRE_SORT(same, LEN(same), "all equal");
    REQUIRE_SORT(dups, LEN(dups), "duplicates");
    REQUIRE_SORT(neg, LEN(neg), "negative and extreme keys");
}

static void test_sort_random(void) {
    int a[1001];
    for (int trial = 0; trial < 20; trial++) {
        srand(100 + trial);
        int n = 1 + rand() % 1001;
        for (int i = 0; i < n; i++) {
            a[i] = rand() % 100 - 20;
        }
        REQUIRE_SORT(a, n, "random keys");
    }
}

static void test_sort_large(void) {
    int n = 400000;
    int *a = malloc(n * sizeof(int));
    srand(7);
    for (int i = 0; i < n; i++) {
        a[i] = rand();
    }
    int ok = sort_ok(a, n, "400,000 random keys (must be O(n lg n))");
    free(a);
    REQUIRE(ok, "(see above)");
}

static void test_random_ops(void) {
    enum { RANGE = 50, CAP = 300 };
    int count[RANGE] = {0}, total = 0, arr[CAP];
    Heap *h = heap_create(CAP);
    REQUIRE(h != NULL, "heap_create(%d) returned NULL", CAP);
    srand(2026);
    for (int step = 1; step <= 5000; step++) {
        if (rand() % 3 != 0) {
            int key = rand() % RANGE;
            int ins = heap_insert(h, key);
            if (ins != (total < CAP)) {
                heap_destroy(h);
                FAIL("step %d: heap_insert returned %s with %d of %d"
                     " keys", step, ins ? "true" : "false", total, CAP);
            }
            if (ins) {
                count[key]++;
                total++;
            }
        } else {
            int out = -999;
            int ex = heap_extract_max(h, &out);
            int want = -999;
            for (int k = RANGE - 1; k >= 0 && total > 0; k--) {
                if (count[k] > 0) {
                    want = k;
                    break;
                }
            }
            if (ex != (total > 0) || out != want) {
                heap_destroy(h);
                FAIL("step %d: heap_extract_max gave %s/%d, expected"
                     " %s/%d", step, ex ? "true" : "false", out,
                     total > 0 ? "true" : "false", want);
            }
            if (ex) {
                count[want]--;
                total--;
            }
        }
        int n = heap_to_array(h, arr);
        if (n != total || heap_size(h) != total || !is_max_heap(arr, n)) {
            heap_destroy(h);
            FAIL("step %d: size or max-heap property is wrong", step);
        }
    }
    heap_destroy(h);
}

static void test_build_random(void) {
    enum { N = 1000 };
    int a[N], sorted[N], out[N];
    srand(313);
    for (int i = 0; i < N; i++) {
        a[i] = rand() % 500;
    }
    memcpy(sorted, a, sizeof(a));
    qsort(sorted, N, sizeof(int), cmp_int);
    Heap *h = heap_build(a, N, N);
    REQUIRE(h != NULL, "heap_build returned NULL");
    heap_to_array(h, out);
    if (!is_max_heap(out, N)) {
        heap_destroy(h);
        FAIL("heap_build on 1000 random keys: not a max-heap");
    }
    for (int i = N - 1; i >= 0; i--) {
        int key = -1;
        if (!heap_extract_max(h, &key) || key != sorted[i]) {
            heap_destroy(h);
            FAIL("extracting all keys after heap_build: key %d is %d,"
                 " expected %d", N - i, key, sorted[i]);
        }
    }
    heap_destroy(h);
}

static void test_to_array(void) {
    MAKE_HEAP(h, 10, SEQ);
    int a[9], b[9];
    int n1 = heap_to_array(h, a);
    int n2 = heap_to_array(h, b);
    int size = heap_size(h);
    heap_destroy(h);
    REQUIRE(n1 == 9 && n2 == 9, "heap_to_array returned %d, expected 9",
            n1);
    REQUIRE(size == 9, "heap_to_array changed the size");
    REQUIRE(same_array(a, b, 9), "two heap_to_array calls differ");
}

/* Not a scored test by itself: the autograder runs it under valgrind. */
static void test_valgrind_scenario(void) {
    MAKE_HEAP(h, 20, SEQ);
    int out;
    for (int i = 0; i < 4; i++) {
        heap_extract_max(h, &out);
    }
    heap_destroy(h);
    Heap *g = heap_build(SEQ, 9, 12);
    REQUIRE(g != NULL, "heap_build returned NULL");
    heap_insert(g, 99);
    heap_extract_max(g, &out);
    heap_destroy(g);
    int a[9];
    memcpy(a, SEQ, sizeof(a));
    heap_sort(a, 9);
}

/* ---- Test table ------------------------------------------------------- */

enum { SHOWN, HIDDEN };

static const struct {
    const char *name;
    int kind;
    void (*fn)(void);
    const char *desc;
} TESTS[] = {
    {"empty_contract", SHOWN, test_empty_contract,
     "new heap is empty; peek/extract on empty return false"},
    {"insert_order", SHOWN, test_insert_order,
     "heap array after inserting 10,24,3,57,4,67,37,87,7"},
    {"peek", SHOWN, test_peek, "peek returns the max and changes nothing"},
    {"extract_order", SHOWN, test_extract_order,
     "extract_max returns keys in decreasing order"},
    {"insert_full", SHOWN, test_insert_full,
     "insert on a full heap returns false and changes nothing"},
    {"build_heap", SHOWN, test_build_heap,
     "heap_build exact array on [10,24,3,57,4,67,37,87,7]"},
    {"heap_sort", SHOWN, test_heap_sort, "heap_sort on 9 and 20 keys"},
    {"create_capacity", SHOWN, test_create_capacity,
     "capacity < 1 gives NULL; capacity 1 works"},
    {"extract_layout", SHOWN, test_extract_layout,
     "heap array after one extract"},
    {"mixed_ops", SHOWN, test_mixed_ops, "interleaved inserts and extracts"},
    {"extract_six", SHOWN, test_extract_six,
     "heap array after 6 extracts"},
    {"empty_then_reuse", SHOWN, test_empty_then_reuse,
     "empty the heap, then use it again"},
    {"build_idempotent", SHOWN, test_build_idempotent,
     "building from a heap leaves it unchanged"},
    {"build_small", SHOWN, test_build_small, "heap_build with n = 0, 1, 2"},
    {"build_args", SHOWN, test_build_args,
     "heap_build argument checks and spare capacity"},
    {"ties", SHOWN, test_ties, "equal children: left child wins"},
    {"sort_edge", SHOWN, test_sort_edge,
     "heap_sort edge cases, duplicates, negatives"},
    {"sort_random", SHOWN, test_sort_random,
     "heap_sort on 20 random arrays"},
    {"sort_large", SHOWN, test_sort_large,
     "heap_sort on 400,000 keys within the time limit"},
    {"random_ops", SHOWN, test_random_ops,
     "5000 random inserts/extracts checked against a model"},
    {"build_random", SHOWN, test_build_random,
     "heap_build on 1000 random keys, then extract all"},
    {"to_array", SHOWN, test_to_array,
     "heap_to_array returns size and changes nothing"},
    {"valgrind_scenario", HIDDEN, test_valgrind_scenario,
     "scenario run under valgrind"},
};

#define NTESTS LEN(TESTS)

static int find_test(const char *name) {
    for (int i = 0; i < NTESTS; i++) {
        if (strcmp(TESTS[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* Run test t in a child process with a time limit. Return 1 if it passes. */
static int run_isolated(int t) {
    fflush(stdout);
    pid_t pid = fork();
    if (pid == 0) {
        alarm(TIME_LIMIT);
        failed = 0;
        TESTS[t].fn();
        fflush(stdout);
        _exit(failed ? 1 : 0);
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status) == 0;
    }
    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM) {
        printf("    timed out after %d seconds (infinite loop?)\n",
               TIME_LIMIT);
    } else if (WIFSIGNALED(status)) {
        printf("    crashed with signal %d (%s)\n", WTERMSIG(status),
               strsignal(WTERMSIG(status)));
    }
    return 0;
}

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc > 1 && strcmp(argv[1], "--list") == 0) {
        for (int i = 0; i < NTESTS; i++) {
            if (TESTS[i].kind != HIDDEN) {
                printf("%s\t%s\n", TESTS[i].name, TESTS[i].desc);
            }
        }
        return 0;
    }
    if (argc > 1) {
        int t = find_test(argv[1]);
        if (t < 0) {
            printf("no test named %s (try ./test_heap --list)\n", argv[1]);
            return 2;
        }
        failed = 0;
        TESTS[t].fn();
        if (!failed) {
            printf("passed\n");
        }
        return failed ? 1 : 0;
    }
    int all = 0, all_pass = 0;
    for (int i = 0; i < NTESTS; i++) {
        if (TESTS[i].kind == HIDDEN) {
            continue;
        }
        printf("%-18s %s\n", TESTS[i].name, TESTS[i].desc);
        int pass = run_isolated(i);
        printf("    %s\n", pass ? "PASS" : "FAIL");
        all++;
        all_pass += pass;
    }
    printf("\ntests passed: %d of %d\n", all_pass, all);
    printf("(Gradescope also counts your 3 student tests, a valgrind"
           " check, and REPORT.md.)\n");
    return all_pass == all ? 0 : 1;
}

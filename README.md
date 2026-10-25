# P2: A max-heap priority queue

CS 313, Fall 2026. Posted Sun 10/18.

- **First step (on your own, or in lab Tue 10/20):** fill in `heap_create`, `heap_destroy`, `heap_size`, and `heap_to_array`. Submit `heap.c` to Gradescope, and check that the `empty_contract` test passes. This makes sure your setup and submission work well before the deadline.
- **Due:** Fri 10/23, 11:59pm. Free extension to Sat 10/24, 11:59pm.
- **Language:** C. You may use AI tools as a tutor when you get stuck, but you need to understand your code: you will explain it in `REPORT.md`, and exams include heap traces and questions about code like this.
- **Set up first:** follow `SETUP.md`. You did this for P1; if you skipped it, do it now. Everything below assumes that setup.
- **Why now:** Midterm 1 (Mon 10/26) has heap traces, and the lab on 10/27 is a bug hunt on heap code like yours.

## The idea

A priority queue hands back the largest key first. A binary max-heap does this with insert and extract-max in O(lg n) time. It stores a complete binary tree in a plain array, with no pointers. You build one in C, following CLRS chapter 6.

**Indexing.** The root is at index 0, because C arrays start at 0. For index i:

- parent(i) = (i − 1) / 2
- left(i) = 2i + 1
- right(i) = 2i + 2

CLRS puts the root at 1 (parent i/2, children 2i and 2i+1). Both rows are on the course reference page. P2 uses the "root at 0" row.

**Fixed capacity.** You give the capacity when you create the heap. It never grows. Insert on a full heap returns `false`.

**Ties.** The tests check exact arrays, so follow CLRS exactly.

- Sift-up swaps a key with its parent only if the key is strictly greater.
- Sift-down compares with strict `>`: first the left child against the node, then the right child against the larger so far. So the **left** child wins when the two children are equal, and a node that equals its larger child stays put.

## Files

| File | What to do |
|---|---|
| `heap.h` | The API and the rules. Read it. Do not edit it. |
| `heap.c` | Fill in every TODO. **Submit.** |
| `test_student.c` | Write three tests (below). **Submit.** |
| `REPORT.md` | Answer two short questions about your code. **Submit.** |
| `test_heap.c` | The autograder's tests, for local runs. Do not edit. |
| `explore.c` | Optional comparison-count driver. Do not edit. |
| `check.h` | Tiny test framework for `test_student.c`. Do not edit. |
| `SETUP.md` | How to set up gcc, make, and valgrind. Same as P1. |
| `.devcontainer/` | Lets GitHub Codespaces set up everything for you (Option C in `SETUP.md`). |
| `Makefile` | `make`, `make check`, `make test`, `make run`, `make valgrind`, `make clean` |

## The API

The `Heap` struct has fields `data`, `size`, `capacity`, and `comparisons`. See `heap.h` for details.

- `heap_create(capacity)`: new empty heap, or NULL if capacity < 1.
- `heap_destroy(h)`: free everything.
- `heap_insert(h, key)`: add a key and sift it up; `false` if full.
- `heap_peek(h, &out)`: read the largest key.
- `heap_extract_max(h, &out)`: remove the largest key; move the last key to the root and sift it down.
- `heap_build(a, n, capacity)`: new heap from an array by BUILD-MAX-HEAP, in O(n). If n is 0, `a` may be NULL, so do not read it then.
- `heap_sort(a, n)`: sort an array in place by HEAPSORT, with no second array.
- `heap_size(h)`: number of keys.
- `heap_to_array(h, out)`: copy the heap array out; the tests use it.
- `heap_comparisons(h)`: key comparisons so far (optional exploration).
- `heap_print(h)`: print the heap level by level. Already written.

`heap.c` gives you `parent`, `left`, `right`, `swap`, and `greater`. You write `sift_up`, `sift_down`, and `build` on a plain `int` array, so `heap_sort` can reuse them. Use loops. You do not need recursion.

**Empty and full rule.** `heap_peek` and `heap_extract_max` return a `bool`. The key comes back through the pointer `out`. On an empty heap they return `false` and leave `*out` unchanged. `heap_insert` on a full heap returns `false` and changes nothing. No function prints, exits, or crashes in these cases. Example:

```c
int top;
if (heap_extract_max(h, &top)) {
    printf("got %d\n", top);
} else {
    printf("heap was empty\n");
}
```

Duplicate keys are allowed.

## Your three tests

In `test_student.c`, write three tests. Each one builds a heap, then checks the whole array with `CHECK_HEAP(h, k0, k1, ...)`. Work out the array **by hand** first, and show the trace in a comment. The keys must be plain numbers, and at least one check per test needs 5 or more keys.

The autograder runs your tests against the instructor's heap. A test passes if all its checks pass there. So a wrong hand trace fails. Run `make test` to try them on your own heap.

## The report

`REPORT.md` asks two short questions: how your `heap_extract_max` and `sift_down` work, and why your `heap_sort` needs no second array. Answer in your own words, about your own code. Replace every `____`.

## Test locally

- `make check` runs the autograder's tests on your `heap.c`. Each test has a 5-second limit.
- `./test_heap insert_order` runs one test. `./test_heap --list` lists them.
- `make test` runs your three tests.
- `make valgrind` checks for leaks, if you have valgrind.

## Optional exploration (not graded)

Every key comparison should go through `greater(x, y, comparisons)`, which counts it. Then run `make run`. For n up to 10^6, it makes a heap from the same keys two ways: n inserts, and one `heap_build`. It prints comparisons per key for random keys and for increasing keys. Which way is O(n)? Why are increasing keys the worst case for inserts? Why are random inserts cheaper than lg n each?

## Grading

P2 is worth 10 points, the same as each of the other programming assignments. Submit `heap.c`, `test_student.c`, and `REPORT.md`.

- **8 points from the autograder.** The points are split evenly across all the tests, and each passing test earns its share. The tests check the empty and full rules, exact heap arrays after inserts, extracts, and `heap_build`, heap sort, argument checks, ties, edge cases, large random runs, no memory leaks (under `valgrind`), your three hand-traced tests, and that `REPORT.md` is filled in. Gradescope shows every test and its message, and you may resubmit as often as you like.
- **2 points graded by the course staff:** readable code (clear names, consistent indentation, comments where the logic is not obvious, no leftover debugging output), the required methods (`heap_sort` sorts in place with no second array or library sort, and `build` works bottom-up), and correct explanations in `REPORT.md` of how your extract-max and heap sort work.

Working code is expected: if you are stuck, office hours, the Canvas discussion, and AI tools can all help you get there.

The autograder also runs your code with AddressSanitizer. If a test says `heap-buffer-overflow`, you read or wrote past the end of an array. The lines below it name the function and line number. In a heap, this usually means a child index was not checked against n.

## Check your understanding

Do these on paper before you code. They are midterm-style.

1. Trace `heap_build` on `[10, 24, 3, 57, 4, 67, 37, 87, 7]`. You should get `[87, 57, 67, 24, 4, 3, 37, 10, 7]`.
2. Insert the same keys one at a time into an empty heap. Why is the array different from question 1?
3. In sift-down, why must the child test be `l < n` and not `l <= n`? What goes wrong in heap sort if it is `<=`?
4. Why does `build` start at index n/2 − 1? What are the indices of the leaves?
5. A student writes parent(i) = i / 2 in 0-indexed code. Which insert is the first to compare the new key with the wrong parent?

## Tips

- Compile with `gcc -Wall -g`; the Makefile does this. Fix every warning.
- Call `heap_print(h)` while you debug. Compare it with your paper trace.
- In `heap_extract_max`, shrink `size` before you sift down, so the old last slot is outside the heap.
- Run `make valgrind` before you submit. You want "All heap blocks were freed" and "ERROR SUMMARY: 0 errors". (valgrind does not run on recent Macs; see `SETUP.md`. The autograder checks for leaks either way.)

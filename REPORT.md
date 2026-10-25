# P2 Report

Name: Hadi Almarzooq

Replace each blank (the underscores) with your answer. Explain your own code in your own words.

1. In two to four sentences, explain how your `heap_extract_max` works. Say what `sift_down` does at each step, and why it compares a child's index with `n` before it reads that child.

`heap_extract_max` removes the maximum value at the root by saving it, replacing it with the last element in the array, and shrinking the heap's active size by 1. It then calls `sift_down`, which compares the new root to its children and swaps it with the largest child if that child is strictly greater, repeating this downward until the max-heap property is restored. `sift_down` compares a child's index with `n` (the active heap size) to ensure it does not read past the end of the heap boundary, which prevents out-of-bounds memory access.

2. In one or two sentences, explain why your `heap_sort` needs no second array. Hint: where does each maximum go after the swap?

`heap_sort` operates entirely in place because it repeatedly extracts the maximum root value and swaps it with the last element of the shrinking active heap. This process naturally builds the sorted array from back to front in the exact same memory space, requiring no secondary array.

### Performance Data

| order | n | insert cmps per key | build cmps per key |
| :--- | :--- | :--- | :--- |
| random | 1000 | 2.22 | 1.85 |
| increasing | 1000 | 7.99 | 1.98 |
| random | 10000 | 2.25 | 1.88 |
| increasing | 10000 | 11.36 | 2.00 |
| random | 100000 | 2.28 | 1.88 |
| increasing | 100000 | 14.69 | 2.00 |
| random | 1000000 | 2.28 | 1.88 |
| increasing | 1000000 | 17.95 | 2.00 |

**Explanation:**
The `heap_build` function processes elements bottom-up, meaning the vast majority of nodes (the leaves) travel zero steps; this keeps the total work tightly bounded at $O(n)$ regardless of the input order[cite: 7]. In contrast, calling `heap_insert` sequentially on an increasing array represents the absolute worst-case scenario: it places every single new maximum key at a leaf and forces it to sift all the way up to the root, requiring $O(\log n)$ work per insertion and driving the overall time complexity up to $O(n \log n)$.

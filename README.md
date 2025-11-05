# Huffman Decoder

## Student Information
- NAME: Caleb Clements
- STUDENT ID: 008878539
- COURSE: CS315 - Data Structures
- REPOSITORY LINK: https://github.com/calebwclem/HuffmanDecoder

# COLLABORATION & SOURCES
- I completed this assignment individually.
- No code was shared with or received from other students.
- Primary sources: the Project 4 spec and starter utilities provided by the instructor; course lecture notes; C++ reference for filesystem, vector, string, algorithm
- Any third-party materials were used only for API clarification (e.g., cppreference for standard library functions).
- ChatGPT was used for discusssion on algorithms, some bug fixing, and some of this README.


# IMPLEMENTATION DETAILS

## CLI & File Layout

* **Usage:** `./huff_decode.x <base>.hdr <base>.code`
* **Location:** both inputs are read from `./input_output/`.
* **Output:** `./input_output/<base>.tokens_decoded` (one token per line, final newline).

---

## Module Boundaries

* **`main.cpp`**

    * Performs all filesystem checks (via `utils.h`).
    * Reads the header into `std::vector<std::pair<std::string,std::string>>` of `(word, code)`.
    * Builds the decoder tree with `HuffmanTree::buildFromHeader(...)`.
    * Streams `.code` into `HuffmanTree::decode(...)` and writes `*.tokens_decoded`.
* **`HuffmanTree.{h,cpp}` (decoder)**

    * Pure logic (no filesystem). Returns `bool` and fills an error string on failure.
    * Owns and deletes the tree (destructor recursively frees all nodes).
* **`utils.{h,cpp}`**

    * Reused from the encoder for consistent error codes/messages:

        * `directoryExists`, `regularFileExistsAndIsAvailable`, `canOpenForWriting`, `exitOnError`, etc.

> The decoder does **not** use the encoder’s `Scanner`, `BST`, or `PriorityQueue`.

---

## `HuffmanTree` (Decoder)

### Node Representation

```cpp
struct TreeNode {
  std::string word;   // non-empty only for leaves
  TreeNode* left  = nullptr;
  TreeNode* right = nullptr;
};
```

* **Leaf:** `word` is non-empty.
* **Internal:** `word` is empty; only structure matters for decoding.

### Building the Tree from the Header

`bool buildFromHeader(const std::vector<std::pair<std::string,std::string>>& pairs, std::string& err)`

1. For each `(word, code)`:

    * Start at `root`.
    * For each bit:

        * `'0'` → go/create `left`
        * `'1'` → go/create `right`
        * If you must descend through a **leaf**, that’s a **prefix conflict** → fail.
    * After the last bit, the target child must be **null**; create a **leaf** there with `word`.
2. **Validation / Failures:**

    * Non-binary bit (not `'0'`/`'1'`).
    * Duplicate code (target slot already occupied).
    * Prefix conflicts (existing code is a prefix of a new one, or vice versa).
3. **Edge Cases:**

    * Empty header → empty tree (valid).
    * Single-symbol corpus (often code `"0"`).

### Decoding the Bitstream

`bool decode(std::istream& code_in, std::ostream& out, std::string& err) const`

* Read the `.code` stream **one character at a time**.
* Ignore `'\n'`/`'\r'`. Any other non-`'0'`/`'1'` character → error.
* Traverse from root: `'0'`→left, `'1'`→right.
* On reaching a **leaf**, write `word` + `'\n'`, then reset to root.
* **Truncated stream:** if EOF occurs while **not** at root (mid-branch), report a truncation error.
* Always produce a final newline in the output file.

---

## Complexity

* **Build:** `O(Σ code_length)` over all header entries.
* **Decode:** `O(#bits)`; each bit processed once, each decoded word emitted once.

---

## Determinism & Correctness

* Header pairs completely determine the tree; order doesn’t matter (conflicts are deterministically detected).
* To verify correctness against the encoder’s output:

  ```bash
  diff input_output/<base>.tokens_decoded input_output/<base>.tokens
  # (no output from diff means files are identical)
  ```

---

## Input Validation & Error Handling

* **`main.cpp`** (using `utils.h`):

    * Ensure `./input_output` exists.
    * Ensure both inputs are readable; ensure output is writable.
    * Spec sanity checks:

        * First non-empty line of `.hdr` begins with a **letter**.
        * First non-newline character in `.code` is **`0` or `1`**.
* **`HuffmanTree`** returns `false` with a human-readable `err` on structural/content issues (non-binary bit, duplicates, prefixes, truncated stream, invalid chars).

---

## Edge Cases Covered

* Empty header & empty code → empty output (success).
* Single-symbol header (e.g., `hello 0`) with long runs of `0` → many `hello` lines.
* Newlines sprinkled throughout `.code` → ignored.
* Any non-binary character in `.code` → error.
* Header with duplicate/prefix issues → error.



# TESTING & STATUS
Everything is working as expected and complies with the overall requirements of the 
assignment for Project 4: Huffman Decoder. The project runs with the provided scripts on Blue.

## TO BUILD

```bash
g++ -std=c++20 -Wall *.cpp -o huffman_decoder
```

## TO RUN
```bash
./huffman_decoder TheBells.hdr TheBells.code
```

*Note: the .hdr and .code files must be in the input_output directory for the test to pass.*

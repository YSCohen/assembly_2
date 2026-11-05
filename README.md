# COM2113 Homework: Assembly 10–11

## Objective

Implement array, struct, and heap operations in x86-64 AT&T syntax using only the techniques shown in Assembly Parts 1-4:

- array elements as contiguous memory;
- address calculation using a base address, index, and element size;
- 4-byte `int` values versus 8-byte `long` values;
- struct fields at fixed offsets;
- struct padding and array-of-struct stride;
- pointers passed in argument registers;
- calling `malloc` and checking for `NULL`;
- returning a heap pointer;
- calling `free` when ownership ends.

## File you may edit

Edit only:

```text
hw1011.s
```

## Build and test

```bash
make test
```

Individual problems:

```bash
make test-p1
make test-p2
make test-p3
make test-p4
make test-p5
make test-p6
```

Allocation-failure tests:

```bash
make test-alloc
```

Memory checks, when Valgrind is installed:

```bash
make valgrind
```

## Assembly rules

The solution may use only these lecture instructions:

```text
movq  movl  movslq
addq  subq  imulq
cmpq
jmp   je    jne   jl    jle   jg    jge
jb    jbe   ja    jae
pushq call  leave ret
```

Allowed calls are limited to `sensor_score`, `malloc`, and `free`, and only in the functions that require them.

Not allowed:

- `leaq`;
- scaled-index operands such as `(%rdi,%rax,8)`;
- `cmov*`, `set*`, or `loop`;
- `calloc`, `realloc`, `memcpy`, or `memmove`;
- writable global data;
- returning a pointer to a local stack object;
- any instruction not listed above.

Calculate addresses in the explicit lecture style: copy an index, multiply by the element size, then add the base address.

---

## Struct layout

The supplied header defines:

```c
typedef struct Sensor {
    int  id;
    long reading;
    int  weight;
} Sensor;
```

Use this x86-64 layout:

| Field | Offset | Width |
|---|---:|---:|
| `id` | `0` | 4 bytes |
| padding | `4` | 4 bytes |
| `reading` | `8` | 8 bytes |
| `weight` | `16` | 4 bytes |
| tail padding | `20` | 4 bytes |

Therefore, one `Sensor` occupies 24 bytes, and neighboring elements of a `Sensor` array are 24 bytes apart.

---

## Problem 1 — Sum positive `int` elements

Implement:

```c
long sum_positive_ints(const int *a, long n);
```

Preconditions:

- `n >= 0`;
- when `n > 0`, `a` points to at least `n` valid `int` elements;
- when `n == 0`, `a` may be `NULL`.

Inspect exactly `a[0]` through `a[n - 1]`. Add only elements strictly greater than zero. Return a 64-bit `long` total. Do not modify the array.

### Example 1

```text
sum_positive_ints([4, -2, 7, 0, -99, 13], 6) -> 24
```

```text
start total = 0
4 is positive:   total = 0 + 4  = 4
-2 is ignored:   total = 4
7 is positive:   total = 4 + 7  = 11
0 is ignored:    total = 11
-99 is ignored:  total = 11
13 is positive:  total = 11 + 13 = 24
```

### Example 2

```text
sum_positive_ints([-5, -1, 0], 3) -> 0
```

No element is strictly positive, so the total never changes from zero.

### Example 3

```text
sum_positive_ints([2000000000, 2000000000, -1], 3) -> 4000000000
```

The first two values are positive:

```text
2,000,000,000 + 2,000,000,000 = 4,000,000,000
```

The negative value is ignored. The result requires a 64-bit total even though each array element is a 32-bit `int`.

### Example 4

```text
sum_positive_ints(NULL, 0) -> 0
```

There are no elements, so the function must not read through the pointer and returns zero.

---

## Problem 2 — Reverse a `long` array in place

Implement:

```c
void reverse_longs(long *a, long n);
```

Preconditions:

- `n >= 0`;
- when `n > 0`, `a` points to at least `n` valid `long` elements;
- when `n == 0`, `a` may be `NULL`.

Reverse the first `n` elements in the existing array. Do not allocate another array.

### Example 1

```text
[1, 2, 3, 4, 5, 6] -> [6, 5, 4, 3, 2, 1]
```

```text
swap 1 and 6: [6, 2, 3, 4, 5, 1]
swap 2 and 5: [6, 5, 3, 4, 2, 1]
swap 3 and 4: [6, 5, 4, 3, 2, 1]
```

### Example 2

```text
[-3, 8, 11, 0, 5] -> [5, 0, 11, 8, -3]
```

```text
swap -3 and 5: [5, 8, 11, 0, -3]
swap 8 and 0:  [5, 0, 11, 8, -3]
```

The middle element `11` is already in its final position.

### Example 3

```text
[42] -> [42]
```

A one-element array needs no swap.

### Example 4

```text
reverse_longs(NULL, 0)
```

There are no elements, so return without reading or writing through the pointer.

---

## Problem 3 — Read one padded struct

Implement:

```c
long sensor_score(const Sensor *s);
```

Precondition: `s` points to one valid `Sensor`.

Return:

```text
(long)s->id + s->reading * (long)s->weight
```

### Example 1

```text
Sensor { id = 7, reading = 20, weight = 3 }
score -> 67
```

```text
reading * weight = 20 * 3 = 60
id + product = 7 + 60 = 67
```

### Example 2

```text
Sensor { id = -9, reading = -12, weight = 5 }
score -> -69
```

```text
reading * weight = -12 * 5 = -60
id + product = -9 + -60 = -69
```

### Example 3

```text
Sensor { id = 4, reading = 10000000000, weight = -2 }
score -> -19999999996
```

```text
10,000,000,000 * -2 = -20,000,000,000
4 + -20,000,000,000 = -19,999,999,996
```

This example checks that `reading` and the final computation remain 64-bit.

---

## Problem 4 — Sum an array of padded structs

Implement:

```c
long sensor_array_total(const Sensor *a, long n);
```

Preconditions:

- `n >= 0`;
- when `n > 0`, `a` points to at least `n` valid `Sensor` objects;
- when `n == 0`, `a` may be `NULL`.

For each element, call the provided `sensor_score` function exactly once and add its result to the total. Adjacent `Sensor` objects are 24 bytes apart.

### Example 1

```text
[
  { id=1,  reading=10, weight=2 },
  { id=-4, reading=7,  weight=-3 },
  { id=9,  reading=-8, weight=5 }
]
```

The individual scores are:

```text
1 + 10*2   = 21
-4 + 7*-3  = -25
9 + -8*5   = -31
```

Therefore:

```text
sensor_array_total(..., 3) -> 21 + -25 + -31 = -35
```

### Example 2

Using only the first two sensors from Example 1:

```text
sensor_array_total(..., 2) -> 21 + -25 = -4
```

### Example 3

```text
sensor_array_total(NULL, 0) -> 0
```

No helper calls or memory reads are needed.

---

## Problem 5 — Return a heap-allocated arithmetic sequence

Implement:

```c
long *make_arithmetic_sequence(long start, long step, long n);
```

If `n <= 0`, return `NULL` without calling `malloc`.

Otherwise, allocate space for exactly `n` `long` elements. If `malloc` returns `NULL`, return `NULL`. On success, fill the array so that:

```text
result[i] = start + i * step
```

The caller owns the returned array and will free it.

### Example 1

```text
make_arithmetic_sequence(10, 3, 4)
-> [10, 13, 16, 19]
```

```text
i=0: 10 + 0*3 = 10
i=1: 10 + 1*3 = 13
i=2: 10 + 2*3 = 16
i=3: 10 + 3*3 = 19
```

### Example 2

```text
make_arithmetic_sequence(5, -2, 5)
-> [5, 3, 1, -1, -3]
```

```text
i=0: 5 + 0*(-2) = 5
i=1: 5 + 1*(-2) = 3
i=2: 5 + 2*(-2) = 1
i=3: 5 + 3*(-2) = -1
i=4: 5 + 4*(-2) = -3
```

### Example 3

```text
make_arithmetic_sequence(8, 0, 3)
-> [8, 8, 8]
```

A step of zero means every element equals the starting value.

### Example 4

```text
make_arithmetic_sequence(10, 3, 0) -> NULL
```

The requested sequence has no elements, so no allocation is made.

---

## Problem 6 — Allocate and destroy one heap struct

Implement both functions:

```c
Sensor *make_sensor(int id, long reading, int weight);
void sensor_destroy(Sensor *s);
```

### `make_sensor`

Allocate exactly one `Sensor` object on the heap.

- If `malloc` returns `NULL`, return `NULL`.
- Otherwise, store the three arguments in the corresponding fields and return the pointer.

### `sensor_destroy`

- If `s == NULL`, return immediately.
- Otherwise, call `free(s)` exactly once.

### Example 1

```text
make_sensor(7, 20, 3)
```

The returned object must contain:

```text
id = 7
reading = 20
weight = 3
```

Calling `sensor_score` on that object gives:

```text
7 + 20*3 = 67
```

The caller later passes the returned pointer to `sensor_destroy`.

### Example 2

```text
make_sensor(-4, 9000000000, -2)
```

The returned object must preserve all three values exactly, including the 64-bit `reading`. Its score is:

```text
-4 + 9,000,000,000 * -2
= -4 + -18,000,000,000
= -18,000,000,004
```

### Example 3

```text
sensor_destroy(NULL)
```

There is no object to free, so the function simply returns.

### Allocation failure

The tests can force `malloc` to fail. In that case, `make_sensor` must return `NULL` and must not attempt to write through the returned pointer.

#include <stddef.h>

void *__real_malloc(size_t size);
void __real_free(void *pointer);

static long malloc_call_number;
static long fail_on_call;
static long successful_allocations;
static long successful_frees;

void alloc_test_reset(long requested_failure_call) {
    malloc_call_number = 0;
    fail_on_call = requested_failure_call;
    successful_allocations = 0;
    successful_frees = 0;
}

long alloc_test_outstanding(void) {
    return successful_allocations - successful_frees;
}

void *__wrap_malloc(size_t size) {
    malloc_call_number++;
    if (fail_on_call > 0 && malloc_call_number == fail_on_call) {
        return NULL;
    }

    void *pointer = __real_malloc(size);
    if (pointer != NULL) successful_allocations++;
    return pointer;
}

void __wrap_free(void *pointer) {
    if (pointer != NULL) successful_frees++;
    __real_free(pointer);
}

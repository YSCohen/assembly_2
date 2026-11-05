#include "../hw1011.h"

#include <stdio.h>

void alloc_test_reset(long fail_call);
long alloc_test_outstanding(void);

static int failures;

#define EXPECT(label, condition)                                                  \
    do {                                                                         \
        if (!(condition)) {                                                       \
            fprintf(stderr, "%s failed\n", (label));                           \
            failures++;                                                          \
        }                                                                         \
    } while (0)

int main(void) {
    alloc_test_reset(1);
    EXPECT("sequence allocation failure returns NULL",
           make_arithmetic_sequence(1, 1, 4) == NULL);
    EXPECT("sequence allocation failure leaks nothing",
           alloc_test_outstanding() == 0);

    alloc_test_reset(1);
    EXPECT("sensor allocation failure returns NULL",
           make_sensor(1, 2, 3) == NULL);
    EXPECT("sensor allocation failure leaks nothing",
           alloc_test_outstanding() == 0);

    alloc_test_reset(0);
    Sensor *sensor = make_sensor(4, 5, 6);
    EXPECT("sensor normal allocation", sensor != NULL);
    if (sensor != NULL) sensor_destroy(sensor);
    EXPECT("sensor_destroy frees the allocation",
           alloc_test_outstanding() == 0);

    alloc_test_reset(0);
    sensor_destroy(NULL);
    EXPECT("sensor_destroy NULL changes nothing",
           alloc_test_outstanding() == 0);

    if (failures != 0) {
        fprintf(stderr, "%d allocation test(s) failed\n", failures);
        return 1;
    }

    puts("allocation-failure tests passed");
    return 0;
}

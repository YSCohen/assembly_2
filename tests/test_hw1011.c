#include "../hw1011.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(sizeof(Sensor) == 24, "Sensor must occupy 24 bytes");
_Static_assert(offsetof(Sensor, id) == 0, "id offset must be 0");
_Static_assert(offsetof(Sensor, reading) == 8, "reading offset must be 8");
_Static_assert(offsetof(Sensor, weight) == 16, "weight offset must be 16");

static int failures;

#define CHECK_EQ(label, actual_expression, expected_expression)                  \
    do {                                                                         \
        long actual_value = (actual_expression);                                 \
        long expected_value = (expected_expression);                             \
        if (actual_value != expected_value) {                                    \
            fprintf(stderr, "%s: expected %ld, got %ld\n",                    \
                    (label), expected_value, actual_value);                       \
            failures++;                                                          \
        }                                                                         \
    } while (0)

#define CHECK_TRUE(label, condition)                                              \
    do {                                                                         \
        if (!(condition)) {                                                       \
            fprintf(stderr, "%s failed\n", (label));                           \
            failures++;                                                          \
        }                                                                         \
    } while (0)

static int long_arrays_equal(const long *a, const long *b, long n) {
    for (long i = 0; i < n; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

static void test_p1(void) {
    int a[] = {4, -2, 7, 0, -99, 13};
    int b[] = {-5, -1, 0};
    int c[] = {2000000000, 2000000000, -1};
    CHECK_EQ("p1 mixed", sum_positive_ints(a, 6), 24);
    CHECK_EQ("p1 none", sum_positive_ints(b, 3), 0);
    CHECK_EQ("p1 wide total", sum_positive_ints(c, 3), 4000000000L);
    CHECK_EQ("p1 empty", sum_positive_ints(NULL, 0), 0);
}

static void test_p2(void) {
    long even[] = {1,2,3,4,5,6};
    const long even_expected[] = {6,5,4,3,2,1};
    reverse_longs(even, 6);
    CHECK_TRUE("p2 even", long_arrays_equal(even, even_expected, 6));

    long odd[] = {-3,8,11,0,5};
    const long odd_expected[] = {5,0,11,8,-3};
    reverse_longs(odd, 5);
    CHECK_TRUE("p2 odd", long_arrays_equal(odd, odd_expected, 5));

    long one[] = {42};
    reverse_longs(one, 1);
    CHECK_EQ("p2 one", one[0], 42);
    reverse_longs(NULL, 0);
}

static long reference_sensor_score(const Sensor *s) {
    return (long)s->id + s->reading * (long)s->weight;
}

static void test_p3(void) {
    Sensor a = {.id = 7, .reading = 20, .weight = 3};
    Sensor b = {.id = -9, .reading = -12, .weight = 5};
    Sensor c = {.id = 4, .reading = 10000000000L, .weight = -2};
    CHECK_EQ("p3 positive", sensor_score(&a), 67);
    CHECK_EQ("p3 signed", sensor_score(&b), -69);
    CHECK_EQ("p3 long", sensor_score(&c), -19999999996L);
}

static void test_p4(void) {
    Sensor sensors[] = {
        {.id = 1, .reading = 10, .weight = 2},
        {.id = -4, .reading = 7, .weight = -3},
        {.id = 9, .reading = -8, .weight = 5},
        {.id = 100, .reading = 0, .weight = 99},
    };
    long expected_all = 0;
    for (long i = 0; i < 4; i++) expected_all += reference_sensor_score(&sensors[i]);
    CHECK_EQ("p4 all", sensor_array_total(sensors, 4), expected_all);
    CHECK_EQ("p4 prefix", sensor_array_total(sensors, 2), -4);
    CHECK_EQ("p4 empty", sensor_array_total(NULL, 0), 0);
}

static void test_p5(void) {
    long *a = make_arithmetic_sequence(10, 3, 4);
    const long expected_a[] = {10,13,16,19};
    CHECK_TRUE("p5 allocation a", a != NULL);
    if (a != NULL) {
        CHECK_TRUE("p5 values a", long_arrays_equal(a, expected_a, 4));
        free(a);
    }

    long *b = make_arithmetic_sequence(5, -2, 5);
    const long expected_b[] = {5,3,1,-1,-3};
    CHECK_TRUE("p5 allocation b", b != NULL);
    if (b != NULL) {
        CHECK_TRUE("p5 values b", long_arrays_equal(b, expected_b, 5));
        free(b);
    }

    long *c = make_arithmetic_sequence(8, 0, 3);
    const long expected_c[] = {8,8,8};
    CHECK_TRUE("p5 allocation c", c != NULL);
    if (c != NULL) {
        CHECK_TRUE("p5 values c", long_arrays_equal(c, expected_c, 3));
        free(c);
    }

    CHECK_TRUE("p5 zero", make_arithmetic_sequence(1, 1, 0) == NULL);
    CHECK_TRUE("p5 negative", make_arithmetic_sequence(1, 1, -3) == NULL);
}

static void test_p6(void) {
    Sensor *a = make_sensor(7, 20, 3);
    CHECK_TRUE("p6 allocation a", a != NULL);
    if (a != NULL) {
        CHECK_EQ("p6 id a", a->id, 7);
        CHECK_EQ("p6 reading a", a->reading, 20);
        CHECK_EQ("p6 weight a", a->weight, 3);
        CHECK_EQ("p6 score a", sensor_score(a), 67);
        sensor_destroy(a);
    }

    Sensor *b = make_sensor(-4, 9000000000L, -2);
    CHECK_TRUE("p6 allocation b", b != NULL);
    if (b != NULL) {
        CHECK_EQ("p6 id b", b->id, -4);
        CHECK_EQ("p6 reading b", b->reading, 9000000000L);
        CHECK_EQ("p6 weight b", b->weight, -2);
        CHECK_EQ("p6 score b", sensor_score(b), -18000000004L);
        sensor_destroy(b);
    }

    sensor_destroy(NULL);
}

static int run_named(const char *name) {
    if (strcmp(name, "p1") == 0) test_p1();
    else if (strcmp(name, "p2") == 0) test_p2();
    else if (strcmp(name, "p3") == 0) test_p3();
    else if (strcmp(name, "p4") == 0) test_p4();
    else if (strcmp(name, "p5") == 0) test_p5();
    else if (strcmp(name, "p6") == 0) test_p6();
    else {
        fprintf(stderr, "unknown test group: %s\n", name);
        return 2;
    }
    return failures == 0 ? 0 : 1;
}

int main(int argc, char **argv) {
    if (argc == 2) {
        int result = run_named(argv[1]);
        if (result == 0) printf("%s passed\n", argv[1]);
        return result;
    }

    test_p1();
    test_p2();
    test_p3();
    test_p4();
    test_p5();
    test_p6();

    if (failures != 0) {
        fprintf(stderr, "%d test(s) failed\n", failures);
        return 1;
    }

    puts("all functional tests passed");
    return 0;
}

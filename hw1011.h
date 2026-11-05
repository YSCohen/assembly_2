#ifndef HW1011_H
#define HW1011_H

typedef struct Sensor {
    int id;
    long reading;
    int weight;
} Sensor;

long sum_positive_ints(const int *a, long n);
void reverse_longs(long *a, long n);
long sensor_score(const Sensor *s);
long sensor_array_total(const Sensor *a, long n);
long *make_arithmetic_sequence(long start, long step, long n);
Sensor *make_sensor(int id, long reading, int weight);
void sensor_destroy(Sensor *s);

#endif

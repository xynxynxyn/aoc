#include <bits/pthreadtypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

struct Sensor {
    int32_t x;
    int32_t y;
    int32_t radius;
};

struct Beacon {
    int32_t x;
    int32_t y;
};

bool beacon_at(struct Beacon* beacons, size_t beacons_len, int32_t x, int32_t y)
{
    for (size_t i = 0; i < beacons_len; i++) {
        if (beacons[i].x == x && beacons[i].y == y) {
            return true;
        }
    }

    return false;
}

int32_t manhattan(int32_t x, int32_t y, int32_t other_x, int32_t other_y)
{
    return abs(x - other_x) + abs(y - other_y);
}

bool inside_sensor_range(struct Sensor* sensors, size_t sensors_len, int32_t x, int32_t y, int32_t* next_possibly_free_x)
{
    for (size_t i = 0; i < sensors_len; i++) {
        struct Sensor sensor = sensors[i];
        if (sensor.radius >= manhattan(x, y, sensor.x, sensor.y)) {
            *next_possibly_free_x = sensor.x + (sensor.radius - abs(sensor.y - y));
            return true;
        }
    }

    return false;
}

void compute_coverage(bool* result, size_t result_len, struct Sensor* sensors, size_t sensors_len, struct Beacon* beacons, size_t beacons_len, int32_t row)
{
    for (size_t i = 0; i < sensors_len; i++) {
        struct Sensor sensor = sensors[i];
        if (row < sensor.y + sensor.radius && row > sensor.y - sensor.radius) {
            // inside the influence of a sensor
            // calculate the width of the circle at this row
            int32_t width = (sensor.radius - abs(sensor.y - row));
            // printf("intersect with sensor(x: %d,y: %d,r: %d)\n", sensor.x - x_offset, sensor.y - y_offset, sensor.radius);
            // printf("  [%d, %d]\n", sensor.x - width + x_offset, sensor.x + width - x_offset);
            for (int32_t x = sensor.x - width; x <= sensor.x + width; x++) {
                result[x] = true;
            }
        }
    }

    // check if there's a beacon on the row, then remove that spot from the list of possible beacon locations
    for (size_t j = 0; j < beacons_len; j++) {
        if (beacons[j].y == row) {
            result[beacons[j].x] = false;
        }
    }
}

int32_t no_beacons_count(struct Sensor* sensors, size_t sensors_len, struct Beacon* beacons, size_t beacons_len, int32_t row)
{

    size_t coverage_len = 10000000;
    bool* coverage = calloc(coverage_len, sizeof(*coverage));
    compute_coverage(coverage, coverage_len, sensors, sensors_len, beacons, beacons_len, row);

    uint32_t counter = 0;
    for (size_t i = 0; i < 10000000; i++) {
        if (coverage[i]) {
            counter++;
        }
    }

    free(coverage);
    return counter;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: aoc [FILE]\n");
        return 1;
    }

    char* file_name = argv[1];
    FILE* fp = fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "could not open file %s\n", file_name);
        return 1;
    }

    const size_t buffer_size = 256;
    char buffer[buffer_size];
    struct Sensor sensors[1024] = { 0 };
    size_t sensors_len = 0;
    struct Beacon beacons[1024] = { 0 };
    size_t beacons_len = 0;
    const int32_t x_offset = 5000000;
    const int32_t y_offset = 0;
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
        int32_t sensor_x, sensor_y, beacon_x, beacon_y;
        if (sscanf(buffer, "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &sensor_x, &sensor_y, &beacon_x, &beacon_y) != 4) {
            fprintf(stderr, "could not parse %s\n", buffer);
            exit(1);
        }
        sensor_x += x_offset;
        sensor_y += y_offset;
        beacon_x += x_offset;
        beacon_y += y_offset;

        size_t manhattan_distance = abs(sensor_x - beacon_x) + abs(sensor_y - beacon_y);
        sensors[sensors_len].x = sensor_x;
        sensors[sensors_len].y = sensor_y;
        sensors[sensors_len].radius = manhattan_distance;
        sensors_len++;

        beacons[beacons_len].x = beacon_x;
        beacons[beacons_len].y = beacon_y;
        beacons_len++;
    }

    printf("can't be in %d spots in row 2000000\n", no_beacons_count(sensors, sensors_len, beacons, beacons_len, 2000000 + y_offset));

    int32_t area_width = 4000000;

    for (int32_t y = 0; y <= area_width; y++) {
        int32_t row = y + y_offset;
        int32_t min_x = 0 + x_offset;
        int32_t max_x = area_width + x_offset;

        for (int32_t x = min_x; x <= max_x; x++) {
            int32_t last_occupied_x;
            if (!inside_sensor_range(sensors, sensors_len, x, y, &last_occupied_x)) {
                int64_t frequency = (int64_t)(x - x_offset) * (int64_t)(area_width + row - y_offset);
                printf("distress beacon at (%d, %d)\n  frequency %ld\n", x - x_offset, y - y_offset, frequency);
                goto done;
            }

            x = last_occupied_x;
        }
    }
done:

    fclose(fp);
    return 0;
}

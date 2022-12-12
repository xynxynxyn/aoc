#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAP_WIDTH 256
#define MAP_HEIGHT 256

bool is_traversable(char from, char to)
{
    return to <= from || from + 1 == to;
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

    char map[MAP_HEIGHT][MAP_WIDTH] = { 0 };
    // Initialise distance map to -1s
    int32_t distance[MAP_HEIGHT][MAP_WIDTH];
    for (size_t y = 0; y < MAP_HEIGHT; y++) {
        for (size_t x = 0; x < MAP_WIDTH; x++) {
            distance[y][x] = -1;
        }
    }

    const size_t buffer_size = 256;
    char buffer[buffer_size];
    size_t height = 0;
    size_t source_x, source_y;
    size_t target_x, target_y;
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
        strcpy(map[height], buffer);

        char* source = strchr(map[height], 'S');
        char* target = strchr(map[height], 'E');

        if (source != NULL) {
            source_x = strcspn(map[height], "S");
            source_y = height;
            *source = 'a';
        }

        if (target != NULL) {
            target_x = strcspn(map[height], "E");
            target_y = height;
            *target = 'z';
        }

        height++;
    }
    size_t width = strlen(map[0]);

    // distance to target from target is 0
    distance[target_y][target_x] = 0;

    bool changed = true;
    while (changed) {
        changed = false;
        for (size_t y = 0; y < height; y++) {
            // printf("%s\n", map[y]);
            for (size_t x = 0; x < width; x++) {
                // if the current tile is uninitialized we cannot reason about adjacent tiles
                // skip
                if (distance[y][x] == -1) {
                    continue;
                }
                int32_t new_distance = distance[y][x] + 1;

                // check adjacent tiles
                // if they can reach the current tile
                // update them to have the value of current tile +1 if it is lower than their value
                // check north
                if (y > 0) {
                    size_t other_x = x;
                    size_t other_y = y - 1;
                    if (is_traversable(map[other_y][other_x], map[y][x])
                        && (distance[other_y][other_x] == -1 || distance[other_y][other_x] > new_distance)) {
                        changed = true;
                        distance[other_y][other_x] = new_distance;
                    }
                }
                // check east
                if (x < width - 1) {
                    size_t other_x = x + 1;
                    size_t other_y = y;
                    if (is_traversable(map[other_y][other_x], map[y][x])
                        && (distance[other_y][other_x] == -1 || distance[other_y][other_x] > new_distance)) {
                        changed = true;
                        distance[other_y][other_x] = new_distance;
                    }
                }
                // check south
                if (y < height - 1) {
                    size_t other_x = x;
                    size_t other_y = y + 1;
                    if (is_traversable(map[other_y][other_x], map[y][x])
                        && (distance[other_y][other_x] == -1 || distance[other_y][other_x] > new_distance)) {
                        changed = true;
                        distance[other_y][other_x] = new_distance;
                    }
                }
                // check west
                if (x > 0) {
                    size_t other_x = x - 1;
                    size_t other_y = y;
                    if (is_traversable(map[other_y][other_x], map[y][x])
                        && (distance[other_y][other_x] == -1 || distance[other_y][other_x] > new_distance)) {
                        changed = true;
                        distance[other_y][other_x] = new_distance;
                    }
                }
            }
        }
    }

    // Part 1
    if (distance[source_y][source_x] == -1) {
        printf("'E' not reachable from 'S'\n");
    } else {
        printf("distance from 'S' to 'E' %d\n", distance[source_y][source_x]);
    }

    // Part 2
    int32_t min_distance = INT32_MAX;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            if (map[y][x] == 'a' && distance[y][x] != -1) {
                if (min_distance > distance[y][x]) {
                    min_distance = distance[y][x];
                }
            }
        }
    }
    if (min_distance == INT32_MAX) {
        printf("'E' not reachable from an 'a'\n");
    } else {
        printf("min distance from an 'a' %d\n", min_distance);
    }

    fclose(fp);
    return 0;
}

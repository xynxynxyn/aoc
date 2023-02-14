#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCAN_SIZE 20
#define BUBBLES_SIZE 256

struct Coords {
    size_t x;
    size_t y;
    size_t z;
};

int32_t exposed_faces(bool (*is_lava)[SCAN_SIZE][SCAN_SIZE][SCAN_SIZE], struct Coords coords)
{
    int32_t exposed = 0;
    size_t x = coords.x;
    size_t y = coords.y;
    size_t z = coords.z;
    if (z >= SCAN_SIZE - 1 || !(*is_lava)[z + 1][y][x]) {
        exposed++;
    }
    if (z == 0 || !(*is_lava)[z - 1][y][x]) {
        exposed++;
    }
    if (y >= SCAN_SIZE - 1 || !(*is_lava)[z][y + 1][x]) {
        exposed++;
    }
    if (y == 0 || !(*is_lava)[z][y - 1][x]) {
        exposed++;
    }
    if (x >= SCAN_SIZE - 1 || !(*is_lava)[z][y][x + 1]) {
        exposed++;
    }
    if (x == 0 || !(*is_lava)[z][y][x - 1]) {
        exposed++;
    }

    return exposed;
}

// returns true if the given coordinate is inside of a bubble
// returns false if the coordinate is in the space surrounding the droplet
// the initial coordinate used to call this function must NOT be lava
bool inside_bubble(bool (*is_lava)[SCAN_SIZE][SCAN_SIZE][SCAN_SIZE], struct Coords coords, int32_t* surface, bool (*scanned)[SCAN_SIZE][SCAN_SIZE][SCAN_SIZE], int32_t* size)
{
    bool is_bubble = true;
    // if we reach the border and the current point is not lava that means this is NOT an air bubble
    // in that case return false
    if ((coords.x == 0 || coords.x == SCAN_SIZE - 1
            || coords.y == 0 || coords.y == SCAN_SIZE - 1
            || coords.z == 0 || coords.z == SCAN_SIZE - 1)
        && !(*is_lava)[coords.z][coords.y][coords.x]) {
        is_bubble = false;
    }

    // return early if this point is lava or if we already scanned this air point
    if ((*is_lava)[coords.z][coords.y][coords.x] || (*scanned)[coords.z][coords.y][coords.x]) {
        return is_bubble;
    }

    // check how many faces are covered
    int32_t covered = 6 - exposed_faces(is_lava, coords);
    *surface += covered;
    *size += 1;
    (*scanned)[coords.z][coords.y][coords.x] = true;

    // recursively explore spaces, if we reach the border
    // if we do, early return false
    struct Coords up = { .x = coords.x, .y = coords.y + 1, .z = coords.z };
    struct Coords down = { .x = coords.x, .y = coords.y - 1, .z = coords.z };
    struct Coords right = { .x = coords.x + 1, .y = coords.y, .z = coords.z };
    struct Coords left = { .x = coords.x - 1, .y = coords.y, .z = coords.z };
    struct Coords back = { .x = coords.x, .y = coords.y, .z = coords.z + 1 };
    struct Coords front = { .x = coords.x, .y = coords.y, .z = coords.z - 1 };
    if (coords.y < SCAN_SIZE - 1 && !inside_bubble(is_lava, up, surface, scanned, size)) {
        is_bubble = false;
    }
    if (coords.y > 0 && !inside_bubble(is_lava, down, surface, scanned, size)) {
        is_bubble = false;
    }
    if (coords.x < SCAN_SIZE - 1 && !inside_bubble(is_lava, right, surface, scanned, size)) {
        is_bubble = false;
    }
    if (coords.x > 0 && !inside_bubble(is_lava, left, surface, scanned, size)) {
        is_bubble = false;
    }
    if (coords.z < SCAN_SIZE - 1 && !inside_bubble(is_lava, back, surface, scanned, size)) {
        is_bubble = false;
    }
    if (coords.z > 0 && !inside_bubble(is_lava, front, surface, scanned, size)) {
        is_bubble = false;
    }

    return is_bubble;
}

// calculate the surface of a bubble once
// if called again from a different coordinate in the bubble returns 0
// if called from a coordinate in the surrounding air also returns 0
int32_t bubble_surface(bool (*is_lava)[SCAN_SIZE][SCAN_SIZE][SCAN_SIZE], struct Coords coords,
    bool (*scanned)[SCAN_SIZE][SCAN_SIZE][SCAN_SIZE])
{
    // check if the given coordinate is contained in a bubble enclosed by lava
    // or if we already scanned this
    if ((*scanned)[coords.z][coords.y][coords.x] || (*is_lava)[coords.z][coords.y][coords.x]) {
        // point is solid, return
        return 0;
    }

    int32_t surface = 0;
    int32_t size = 0;
    bool inside = inside_bubble(is_lava, coords, &surface, scanned, &size);

    if (!inside) {
        // if not inside a bubble, but on the outside set surface to 0
#ifdef DEBUG
        printf("exterior of size %d with surface %d\n", size, surface);
#endif
        surface = 0;
    } else {
#ifdef DEBUG
        printf("bubble of size %d with surface %d (%zu,%zu,%zu)\n", size, surface, coords.x, coords.y, coords.z);
#endif
    }

#ifdef DEBUG
    printf("%d\n", surface);
#endif

    return surface;
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

    // is_lava[z][y][x]
    bool is_lava[SCAN_SIZE][SCAN_SIZE][SCAN_SIZE] = { 0 };

    const size_t buffer_size = 256;
    char buffer[buffer_size];
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
        size_t x, y, z;
        if (sscanf(buffer, "%zu,%zu,%zu", &x, &y, &z) != 3) {
            fprintf(stderr, "could not parse line %s\n", buffer);
            exit(1);
        }

        if (x >= SCAN_SIZE || y >= SCAN_SIZE || z >= SCAN_SIZE) {
            fprintf(stderr, "scan cube not big enough for %s\n", buffer);
            exit(1);
        }

        is_lava[z][y][x] = true;
    }

    int32_t surface = 0;
    int32_t interior_surface = 0;
    struct Coords bubbles[BUBBLES_SIZE] = { 0 };
    bool air_scanned[SCAN_SIZE][SCAN_SIZE][SCAN_SIZE] = { 0 };
    size_t bubbles_len = 0;
    for (size_t x = 0; x < SCAN_SIZE; x++) {
        for (size_t y = 0; y < SCAN_SIZE; y++) {
            for (size_t z = 0; z < SCAN_SIZE; z++) {
                struct Coords coord = { .x = x, .y = y, .z = z };
                if (is_lava[z][y][x]) {
                    int32_t exposed = exposed_faces(&is_lava, coord);
                    surface += exposed;
                } else {
                    // check air space for whether it's a bubble
                    interior_surface += bubble_surface(&is_lava, coord, &air_scanned);
                }
            }
        }
    }

    printf("part 1: %d\n", surface);
    printf("part 2: %d\n", surface - interior_surface);

    fclose(fp);
    return 0;
}

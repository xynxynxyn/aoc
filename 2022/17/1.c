#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAVE_HEIGHT 32768
#define CAVE_WIDTH 7
#define BUFFER_SIZE 256
#define JET_SIZE 16384
#define SLICES_BUFFER_LEN 64

enum Jet {
    JET_LEFT = '<',
    JET_RIGHT = '>'
};

enum Tile {
    TILE_EMPTY = 0,
    TILE_ROCK = 1,
};

enum Shape {
    // ####
    SHAPE_LINE,
    // .#.
    // ###
    // .#.
    SHAPE_CROSS,
    // ..#
    // ..#
    // ###
    SHAPE_L,
    // #
    // #
    // #
    // #
    SHAPE_I,
    // ##
    // ##
    SHAPE_SQUARE,
};

struct ShapeGenerator {
    enum Shape* shapes;
    size_t shapes_size;
    size_t next;
};

enum Shape shape_gen_next(struct ShapeGenerator* generator)
{
    enum Shape s = generator->shapes[generator->next];
    generator->next++;
    generator->next %= generator->shapes_size;
    return s;
}

void shape_gen_skip(struct ShapeGenerator* gen, size_t skip)
{
    gen->next += skip;
    gen->next %= gen->shapes_size;
}

struct JetGenerator {
    enum Jet* jets;
    size_t jets_size;
    size_t next;
};

enum Jet jet_gen_next(struct JetGenerator* generator)
{
    enum Jet j = generator->jets[generator->next];
    generator->next++;
    generator->next %= generator->jets_size;
    return j;
}

void jet_gen_skip(struct JetGenerator* gen, size_t skip)
{
    gen->next += skip;
    gen->next %= gen->jets_size;
}

struct Cave {
    enum Tile tiles[CAVE_HEIGHT][CAVE_WIDTH];
    size_t tower_height;
    size_t total_tower_height;
    struct ShapeGenerator shape_gen;
    struct JetGenerator jet_gen;
};

struct CaveSlice {
    enum Tile tiles[3][CAVE_WIDTH];
    size_t jet_gen_state;
    size_t shape_gen_state;
    size_t height;
    size_t rocks;
};

// All shapes are anchored in the bottom left corner
bool collides(struct Cave* cave, enum Shape shape, int32_t x, int32_t y)
{
    switch (shape) {
    case SHAPE_LINE:
        return x < 0 || x + 3 >= CAVE_WIDTH || y < 0
            || cave->tiles[y][x] == TILE_ROCK
            || cave->tiles[y][x + 1] == TILE_ROCK
            || cave->tiles[y][x + 2] == TILE_ROCK
            || cave->tiles[y][x + 3] == TILE_ROCK;
    case SHAPE_CROSS:
        return x < 0 || x + 2 >= CAVE_WIDTH || y < 0
            || cave->tiles[y][x + 1] == TILE_ROCK
            || cave->tiles[y + 1][x] == TILE_ROCK
            || cave->tiles[y + 1][x + 1] == TILE_ROCK
            || cave->tiles[y + 1][x + 2] == TILE_ROCK
            || cave->tiles[y + 2][x + 1] == TILE_ROCK;
    case SHAPE_L:
        return x < 0 || x + 2 >= CAVE_WIDTH || y < 0
            || cave->tiles[y][x] == TILE_ROCK
            || cave->tiles[y][x + 1] == TILE_ROCK
            || cave->tiles[y][x + 2] == TILE_ROCK
            || cave->tiles[y + 1][x + 2] == TILE_ROCK
            || cave->tiles[y + 2][x + 2] == TILE_ROCK;
    case SHAPE_I:
        return x < 0 || x >= CAVE_WIDTH || y < 0
            || cave->tiles[y][x] == TILE_ROCK
            || cave->tiles[y + 1][x] == TILE_ROCK
            || cave->tiles[y + 2][x] == TILE_ROCK
            || cave->tiles[y + 3][x] == TILE_ROCK;
    case SHAPE_SQUARE:
        return x < 0 || x + 1 >= CAVE_WIDTH || y < 0
            || cave->tiles[y][x] == TILE_ROCK
            || cave->tiles[y][x + 1] == TILE_ROCK
            || cave->tiles[y + 1][x] == TILE_ROCK
            || cave->tiles[y + 1][x + 1] == TILE_ROCK;
    default:
        fprintf(stderr, "unknown shape %c\n", shape);
        exit(1);
    }
}

int32_t shape_height(enum Shape shape)
{
    switch (shape) {
    case SHAPE_LINE:
        return 1;
    case SHAPE_CROSS:
        return 3;
    case SHAPE_L:
        return 3;
    case SHAPE_I:
        return 4;
    case SHAPE_SQUARE:
        return 2;
    }
}

void place(struct Cave* cave, enum Shape shape, int32_t x, int32_t y)
{
    switch (shape) {
    case SHAPE_LINE:
        cave->tiles[y][x] = TILE_ROCK;
        cave->tiles[y][x + 1] = TILE_ROCK;
        cave->tiles[y][x + 2] = TILE_ROCK;
        cave->tiles[y][x + 3] = TILE_ROCK;
        break;
    case SHAPE_CROSS:
        cave->tiles[y][x + 1] = TILE_ROCK;
        cave->tiles[y + 1][x] = TILE_ROCK;
        cave->tiles[y + 1][x + 1] = TILE_ROCK;
        cave->tiles[y + 1][x + 2] = TILE_ROCK;
        cave->tiles[y + 2][x + 1] = TILE_ROCK;
        break;
    case SHAPE_L:
        cave->tiles[y][x] = TILE_ROCK;
        cave->tiles[y][x + 1] = TILE_ROCK;
        cave->tiles[y][x + 2] = TILE_ROCK;
        cave->tiles[y + 1][x + 2] = TILE_ROCK;
        cave->tiles[y + 2][x + 2] = TILE_ROCK;
        break;
    case SHAPE_I:
        cave->tiles[y][x] = TILE_ROCK;
        cave->tiles[y + 1][x] = TILE_ROCK;
        cave->tiles[y + 2][x] = TILE_ROCK;
        cave->tiles[y + 3][x] = TILE_ROCK;
        break;
    case SHAPE_SQUARE:
        cave->tiles[y][x] = TILE_ROCK;
        cave->tiles[y][x + 1] = TILE_ROCK;
        cave->tiles[y + 1][x] = TILE_ROCK;
        cave->tiles[y + 1][x + 1] = TILE_ROCK;
        break;
    default:
        fprintf(stderr, "unknown shape %c\n", shape);
        exit(1);
    }
}

bool row_is_impassable(struct Cave* cave, int32_t y)
{
    for (size_t x = 0; x < CAVE_WIDTH; x++) {
        // there needs to be an empty space of 3 tiles vertically so something can pass through
        // for example this is impassable
        // #..#.##
        // .......
        // .##.#..
        //
        // if there's a column with 3 empty slots, that means there's a possibility something can slip through
        if (cave->tiles[y][x] != TILE_ROCK
            && cave->tiles[y + 1][x] != TILE_ROCK
            && cave->tiles[y + 2][x] != TILE_ROCK) {
            return false;
        }
    }

    return true;
}

void print_cave(struct Cave* cave)
{
    size_t height = cave->total_tower_height;
    for (int32_t y = cave->tower_height - 1; y >= 0; y--) {
        char index[16] = { 0 };
        strcpy(index, "               ");
        if (height % 10 == 0 || height == 1 || height == cave->total_tower_height) {
            sprintf(index, "%zu", height);
        }
        printf("%15s |", index);

        for (int32_t x = 0; x < CAVE_WIDTH; x++) {
            switch (cave->tiles[y][x]) {
            case TILE_EMPTY:
                printf(" ");
                break;
            case TILE_ROCK:
                printf("@");
                break;
            }
        }
        printf("|\n");
        height--;
    }

    // print bottom
    printf("                +");
    for (size_t i = 0; i < CAVE_WIDTH; i++) {
        printf("-");
    }
    printf("+\n");
}

bool cave_slice_equals(struct Cave* cave, struct CaveSlice* slice, int32_t y)
{
    for (size_t dy = 0; dy < 3; dy++) {
        for (size_t x = 0; x < CAVE_WIDTH; x++) {
            if (cave->tiles[y + dy][x] != slice->tiles[dy][x]) {
                return false;
            }
        }
    }

    return slice->jet_gen_state == cave->jet_gen.next && slice->shape_gen_state == cave->shape_gen.next;
}

void snapshot(struct Cave* cave, struct CaveSlice* slice, int32_t y)
{
    for (size_t dy = 0; dy < 3; dy++) {
        memcpy(slice->tiles[dy], cave->tiles[y + dy], sizeof(cave->tiles[0][0]) * CAVE_WIDTH);
    }
    slice->jet_gen_state = cave->jet_gen.next;
    slice->shape_gen_state = cave->shape_gen.next;
    slice->height = y;
}

void simulate(struct Cave* cave, size_t number_of_rocks)
{
    // keep track of the last 32 slices
    struct CaveSlice slices[SLICES_BUFFER_LEN] = { 0 };
    size_t slices_len = 0;

    // reset the generators
    cave->shape_gen.next = 0;
    cave->jet_gen.next = 0;

#ifdef UPDATE
    size_t one_percent = number_of_rocks / 100;
#endif

    for (size_t r = 0; r < number_of_rocks; r++) {

#ifdef UPDATE
        if (r % one_percent == 0) {
            size_t percentage = r / one_percent;
            printf("\rprogress %3d%%", percentage);
            fflush(stdout);
        }
#endif

        enum Shape cur_shape = shape_gen_next(&cave->shape_gen);

        // insert the shape at its initial coords relative to current tower height
        int32_t x = 2;
        int32_t y = cave->tower_height + 3;

        for (;;) {
            // simulate until the shape is dropped

            // go left or right first, or neither
            enum Jet jet = jet_gen_next(&cave->jet_gen);
            switch (jet) {
            case JET_LEFT:
                if (!collides(cave, cur_shape, x - 1, y)) {
                    x--;
                }
                break;
            case JET_RIGHT:
                if (!collides(cave, cur_shape, x + 1, y)) {
                    x++;
                }
                break;
            default:
                fprintf(stderr, "unknown jet\n");
                exit(1);
            }

            // then drop lower if possible

            if (collides(cave, cur_shape, x, y - 1)) {

                // set in place
                place(cave, cur_shape, x, y);
                // determine new height
                int32_t possible_new_height = y + shape_height(cur_shape);
                if (possible_new_height > cave->tower_height) {
                    cave->total_tower_height += possible_new_height - cave->tower_height;
                    cave->tower_height = possible_new_height;
                }

                // check if we constructed an impassable barrier
                // this means nothing below it every changes
                for (size_t row = y; row < y + shape_height(cur_shape); row++) {
                    if (row_is_impassable(cave, row)) {
                        // check if we've seen this exact state before
                        bool cycle_detected = false;
                        for (size_t i = 0; i < slices_len; i++) {
                            if (cave_slice_equals(cave, &slices[i], row)) {
                                // if this is the case then we can deduce that this cycle will repeat
                                // infinitely, thus we just skip ahead
                                size_t cycle_height = row - slices[i].height;
                                size_t cycle_len = r - slices[i].rocks;

                                size_t rocks_left = number_of_rocks - r;
                                size_t skip_cycles = rocks_left / cycle_len;
                                size_t skip_rocks = skip_cycles * cycle_len;
                                cave->total_tower_height += cycle_height * skip_cycles;
#ifdef DEBUG
                                printf("detected cycle of height %zu\n", cycle_height);
                                printf("begins at %zu and ends at %zu\n", slices[i].height, row);
                                printf("cycle contains %zu rocks\n", cycle_len);
                                print_cave(cave);
                                printf("skipping ahead %zu rocks\n", skip_rocks);
#endif
                                r += skip_rocks;
                                cycle_detected = true;
                                break;
                            }
                        }
                        if (!cycle_detected) {
                            // no cycle was detected, record the current state
                            if (slices_len >= SLICES_BUFFER_LEN) {
                                fprintf(stderr, "cannot store more slices\n");
                                exit(1);
                            }
                            // snapshot and insert the new state
                            snapshot(cave, &slices[slices_len], row);
                            slices[slices_len].rocks = r;
                            slices_len++;
                        }

                        break;
                    }
                }

                break;
            } else {
                // let the rock fall 1 tile
                y--;
            }
        }
    }
#ifdef UPDATE
    printf("\n");
#endif
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

    char buffer[BUFFER_SIZE] = { 0 };
    enum Jet jets[JET_SIZE] = { 0 };
    size_t jets_size = 0;
    while (fgets(buffer, BUFFER_SIZE, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        char* t = buffer;
        while (*t) {
            jets[jets_size] = *t;
            jets_size++;
            t++;
        }
    }

    enum Shape shapes[] = { SHAPE_LINE, SHAPE_CROSS, SHAPE_L, SHAPE_I, SHAPE_SQUARE };
    struct ShapeGenerator shape_generator = { .shapes = shapes, .shapes_size = sizeof(shapes) / sizeof(*shapes), .next = 0 };
    struct JetGenerator jet_generator = { .jets = jets, .jets_size = jets_size, .next = 0 };
    struct Cave part1_cave = { .jet_gen = jet_generator, .shape_gen = shape_generator };
    struct Cave part2_cave = { .jet_gen = jet_generator, .shape_gen = shape_generator };

    // part 1
    simulate(&part1_cave, 2022);
#ifdef DEBUG
    print_cave(&part1_cave);
#endif
    printf("part 1: %zu\n", part1_cave.total_tower_height);

    // part 2
    simulate(&part2_cave, 1000000000000);
#ifdef DEBUG
    print_cave(&part2_cave);
#endif
    printf("part 2: %zu\n", part2_cave.total_tower_height);

    fclose(fp);
    return 0;
}

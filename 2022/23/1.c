#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 128
#define ELFS_SIZE 4096
struct Position {
    int32_t x;
    int32_t y;
};

struct Elf {
    struct Position position;
};

enum Cardinal { NORTH = 0,
    SOUTH = 1,
    WEST = 2,
    EAST = 3 };

struct CardinalGenerator {
    int32_t cur;
};

enum Cardinal card_gen_next(struct CardinalGenerator* gen)
{
    enum Cardinal ret = gen->cur;
    gen->cur += 1;
    gen->cur %= 4;
    return ret;
}

struct Surroundings {
    bool n;
    bool ne;
    bool e;
    bool se;
    bool s;
    bool sw;
    bool w;
    bool nw;
};

struct Surroundings surroundings(struct Elf* elf, struct Elf* elfs, size_t elfs_len)
{
    int32_t x = elf->position.x;
    int32_t y = elf->position.y;
    struct Surroundings surroundings = { 0 };
    for (size_t e = 0; e < elfs_len; e++) {
        int32_t other_x = elfs[e].position.x;
        int32_t other_y = elfs[e].position.y;
        if (x == other_x && y - 1 == other_y) {
            surroundings.n = true;
        } else if (x + 1 == other_x && y - 1 == other_y) {
            surroundings.ne = true;
        } else if (x + 1 == other_x && y == other_y) {
            surroundings.e = true;
        } else if (x + 1 == other_x && y + 1 == other_y) {
            surroundings.se = true;
        } else if (x == other_x && y + 1 == other_y) {
            surroundings.s = true;
        } else if (x - 1 == other_x && y + 1 == other_y) {
            surroundings.sw = true;
        } else if (x - 1 == other_x && y == other_y) {
            surroundings.w = true;
        } else if (x - 1 == other_x && y - 1 == other_y) {
            surroundings.nw = true;
        }

        // early break if we are completely surrounded
        if (surroundings.n && surroundings.ne && surroundings.e && surroundings.se
                && surroundings.s && surroundings.sw && surroundings.w && surroundings.nw) {
            break;
        }
    }

    return surroundings;
}

bool surroundings_empty(struct Surroundings* surroundings)
{
    return !(surroundings->n || surroundings->ne || surroundings->e || surroundings->se
            || surroundings->s || surroundings->sw || surroundings->w || surroundings->nw);
}

struct Position next_move(struct Elf* elf, struct Surroundings* sur, enum Cardinal consider_first)
{
    struct CardinalGenerator card_gen = { .cur = consider_first };
    for (size_t i = 0; i < 4; i++) {
        enum Cardinal next = card_gen_next(&card_gen);
        switch (next) {
        case NORTH:
            if (!sur->n && !sur->ne && !sur->nw) {
                struct Position new_pos = elf->position;
                new_pos.y--;
                return new_pos;
            }
            break;
        case SOUTH:
            if (!sur->s && !sur->se && !sur->sw) {
                struct Position new_pos = elf->position;
                new_pos.y++;
                return new_pos;
            }
            break;
        case WEST:
            if (!sur->w && !sur->nw && !sur->sw) {
                struct Position new_pos = elf->position;
                new_pos.x--;
                return new_pos;
            }
            break;
        case EAST:
            if (!sur->e && !sur->ne && !sur->se) {
                struct Position new_pos = elf->position;
                new_pos.x++;
                return new_pos;
            }
            break;
        }
    }

    // completely surrounded by elfs, stay where you are
    return elf->position;
}

// simulate one round
// if a single elf has to move, return true
// otherwise return false so we know when to stop
bool simulate_round(struct Elf* elfs, size_t elfs_len, enum Cardinal consider_first)
{
    struct Position* considerations = calloc(elfs_len, sizeof(*considerations));
    bool* should_move = calloc(elfs_len, sizeof(*should_move));
    bool one_has_to_move = false;

    // go through all the elves and check whether they want to and can move
    // by scanning their surroundings
    for (size_t e = 0; e < elfs_len; e++) {
        struct Surroundings sur = surroundings(&elfs[e], elfs, elfs_len);
        if (!surroundings_empty(&sur)) {
            // at least one elf has a neighbor somewhere
            one_has_to_move = true;

            struct Position new_pos = next_move(&elfs[e], &sur, consider_first);
            if (!(new_pos.x == elfs[e].position.x && new_pos.y == elfs[e].position.y)) {
                // mark down the move consideration for this elf if they want to move
                // somewhere, that is not their current position
                should_move[e] = true;
                considerations[e] = new_pos;
            }
        }
    }

    // filter duplicate destinations
    for (size_t i = 0; i < elfs_len; i++) {
        if (should_move[i]) {
            // check if there are any other elfs with a destination the same as ours
            for (size_t j = i + 1; j < elfs_len; j++) {
                if (should_move[j] && considerations[i].x == considerations[j].x
                        && considerations[i].y == considerations[j].y) {
#ifdef DEBUG
                    printf("(%d, %d) and (%d, %d) have same goal\n",
                            elfs[i].position.x, elfs[i].position.y,
                            elfs[j].position.x, elfs[j].position.y);
#endif
                    // if we found one, remove them
                    should_move[i] = false;
                    should_move[j] = false;
                }
            }
        }
    }

    // execute the moves left which are valid
    for (size_t i = 0; i < elfs_len; i++) {
        if (should_move[i]) {
#ifdef DEBUG
            printf("(%d,%d) -> (%d,%d)\n", elfs[i].position.x, elfs[i].position.y,
                    considerations[i].x, considerations[i].y);
#endif
            elfs[i].position.x = considerations[i].x;
            elfs[i].position.y = considerations[i].y;
        }
    }

    free(should_move);
    free(considerations);
    return one_has_to_move;
}

int32_t calculate_area(struct Elf* elfs, size_t elfs_len)
{
    int32_t min_x = INT32_MAX;
    int32_t min_y = INT32_MAX;
    int32_t max_x = INT32_MIN;
    int32_t max_y = INT32_MIN;

    for (size_t i = 0; i < elfs_len; i++) {
        int32_t x = elfs[i].position.x;
        int32_t y = elfs[i].position.y;
        if (x < min_x) {
            min_x = x;
        }
        if (x > max_x) {
            max_x = x;
        }
        if (y < min_y) {
            min_y = y;
        }
        if (y > max_y) {
            max_y = y;
        }
    }

    return (max_x - min_x + 1) * (max_y - min_y + 1);
}

bool position_has_elf(struct Elf* elfs, size_t elfs_len, int32_t x, int32_t y)
{
    for (size_t i = 0; i < elfs_len; i++) {
        if (elfs[i].position.x == x && elfs[i].position.y == y) {
            return true;
        }
    }
    return false;
}

void print_field(struct Elf* elfs, size_t elfs_len)
{
    int32_t min_x = INT32_MAX;
    int32_t min_y = INT32_MAX;
    int32_t max_x = INT32_MIN;
    int32_t max_y = INT32_MIN;
    for (size_t i = 0; i < elfs_len; i++) {
        int32_t x = elfs[i].position.x;
        int32_t y = elfs[i].position.y;
        if (x < min_x) {
            min_x = x;
        }
        if (y < min_y) {
            min_y = y;
        }
        if (x > max_x) {
            max_x = x;
        }
        if (y > max_y) {
            max_y = y;
        }
    }

    for (int32_t y = min_y; y <= max_y; y++) {
        printf("%3d ", y);
        for (int32_t x = min_x; x <= max_x; x++) {
            if (position_has_elf(elfs, elfs_len, x, y)) {
                printf("#");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
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

    char buffer[BUFFER_SIZE];
    int32_t y = 0;
    struct Elf* elfs = calloc(ELFS_SIZE, sizeof(*elfs));
    size_t elfs_len = 0;
    while (fgets(buffer, BUFFER_SIZE, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;

        char* c = buffer;
        int32_t x = 0;
        while (*c) {
            if (*c == '#') {
                if (elfs_len >= ELFS_SIZE) {
                    fprintf(stderr, "more than %d elfs, allocate more space\n", ELFS_SIZE);
                    exit(1);
                }
                elfs[elfs_len].position.x = x;
                elfs[elfs_len].position.y = y;
                elfs_len++;
            }
            c++;
            x++;
        }
        y++;
    }

    struct CardinalGenerator card_gen = { 0 };
    enum Cardinal consider_first = card_gen_next(&card_gen);
    size_t round = 1;
#ifdef DEBUG
    print_field(elfs, elfs_len);
#endif
    while (simulate_round(elfs, elfs_len, consider_first)) {
        if (round == 10) {
            int32_t area = calculate_area(elfs, elfs_len);
            printf("round 10 area: %d = %d - %zu\n", area - (int32_t)elfs_len, area, elfs_len);
        }
        round++;
        consider_first = card_gen_next(&card_gen);
#ifdef DEBUG
        printf("after round %zu\n", round);
        print_field(elfs, elfs_len);
#endif
    }

    printf("finished on round %zu\n", round);

    free(elfs);

    fclose(fp);
    return 0;
}

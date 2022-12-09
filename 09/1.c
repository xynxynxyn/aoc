#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Position {
    int32_t x;
    int32_t y;
};

struct Position* position_copy(struct Position* position)
{
    struct Position* copy = malloc(sizeof(*copy));
    copy->x = position->x;
    copy->y = position->y;
    return copy;
}

struct Set {
    struct Position* positions[10000];
    size_t size;
};

bool set_insert(struct Set* set, struct Position* position)
{
    for (size_t i = 0; i < set->size; i++) {
        struct Position* pos = set->positions[i];
        if (pos->x == position->x && pos->y == position->y) {
            // position already in the set
            return false;
        }
    }

    set->positions[set->size] = position;
    set->size++;
    return true;
}

void update_tail(struct Position* head, struct Position* tail)
{
    int32_t x_diff = head->x - tail->x;
    int32_t y_diff = head->y - tail->y;
    if (x_diff > 1 || x_diff < -1) {
        if (x_diff > 1) {
            tail->x++;
        } else if (x_diff < -1) {
            tail->x--;
        }
        if (y_diff > 0) {
            tail->y++;
        } else if (y_diff < 0) {
            tail->y--;
        }
    } else if (y_diff > 1 || y_diff < -1) {
        if (y_diff > 1) {
            tail->y++;
        } else if (y_diff < -1) {
            tail->y--;
        }
        if (x_diff > 0) {
            tail->x++;
        } else if (x_diff < 0) {
            tail->x--;
        }
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

    const size_t buffer_size = 256;
    char buffer[buffer_size];
    struct Position head = { 0 };
    struct Position tail = { 0 };
    struct Set tail_positions = { 0 };

    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
        char direction;
        size_t steps;
        if (sscanf(buffer, "%c %zu", &direction, &steps) != 2) {
            fprintf(stderr, "could not parse line %s", buffer);
            return 1;
        }

        // simulate each step individually
        for (size_t i = 0; i < steps; i++) {
            switch (direction) {
            case 'U':
                head.y--;
                break;
            case 'R':
                head.x++;
                break;
            case 'D':
                head.y++;
                break;
            case 'L':
                head.x--;
                break;
            default:
                fprintf(stderr, "unknown direction %c", direction);
                return 1;
            }

            update_tail(&head, &tail);

            // insert tail position
            struct Position* tail_pos = position_copy(&tail);
            if (!set_insert(&tail_positions, tail_pos)) {
                // free the position if it's not inserted
                free(tail_pos);
            }
        }
    }

    printf("visited locations: %zu\n", tail_positions.size);

    // free set
    for (size_t i = 0; i < tail_positions.size; i++) {
        free(tail_positions.positions[i]);
    }

    fclose(fp);
    return 0;
}

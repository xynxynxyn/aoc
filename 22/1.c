#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 8128
#define MAP_HEIGHT 2048
#define MAP_WIDTH 2048
#define INSTRUCTIONS_SIZE 8128

enum Tile {
    TILE_EMPTY = 0,
    TILE_OPEN = '.',
    TILE_WALL = '#',
};

enum InstructionType {
    INSTR_MOVE,
    INSTR_TURN,
};

enum Turn {
    TURN_LEFT = 'L',
    TURN_RIGHT = 'R',
};

struct Instruction {
    enum InstructionType type;
    // INSTR_TURN
    enum Turn turn_direction;
    // INSTR_MOVE
    int32_t distance;
};

enum Direction {
    DIR_NORTH = 0,
    DIR_EAST = 1,
    DIR_SOUTH = 2,
    DIR_WEST = 3,
};

struct Position {
    int32_t x;
    int32_t y;
};

struct Actor {
    enum Direction direction;
    struct Position position;
};

struct Position teleport(enum Tile (*map)[MAP_WIDTH], size_t map_width, size_t map_height, struct Actor* actor)
{
    struct Position pos = actor->position;
    switch (actor->direction) {
    case DIR_NORTH:
        for (int32_t y = map_height - 1; y >= 0; y--) {
            if (map[y][pos.x] == TILE_WALL) {
                // ran into a wall on the other side, stop
                return pos;
            } else if (map[y][pos.x] == TILE_OPEN) {
                // found empty spot to teleport to
                pos.y = y;
                return pos;
            }
        }
        break;
    case DIR_EAST:
        for (int32_t x = 0; x < map_width; x++) {
            if (map[pos.y][x] == TILE_WALL) {
                // ran into a wall on the other side, stop
                return pos;
            } else if (map[pos.y][x] == TILE_OPEN) {
                // found empty spot to teleport to
                pos.x = x;
                return pos;
            }
        }
        break;
    case DIR_SOUTH:
        for (int32_t y = 0; y < map_height; y++) {
            if (map[y][actor->position.x] == TILE_WALL) {
                // ran into a wall on the other side, stop
                return pos;
            } else if (map[y][pos.x] == TILE_OPEN) {
                // found empty spot to teleport to
                pos.y = y;
                return pos;
            }
        }
        break;
    case DIR_WEST:
        for (int32_t x = map_width - 1; x >= 0; x--) {
            if (map[pos.y][x] == TILE_WALL) {
                // ran into a wall on the other side, stop
                return pos;
            } else if (map[pos.y][x] == TILE_OPEN) {
                // found empty spot to teleport to
                pos.x = x;
                return pos;
            }
        }
        break;
    }
    fprintf(stderr, "could not teleport from (%d, %d)\n", actor->position.x, actor->position.y);
    exit(1);
}

struct Position step(enum Tile (*map)[MAP_WIDTH], size_t map_width, size_t map_height, struct Actor* actor)
{
    struct Position new_pos = actor->position;
    switch (actor->direction) {
    case DIR_NORTH:
        if (actor->position.y <= 0) {
            return teleport(map, map_width, map_height, actor);
        }
        new_pos.y--;
        break;
    case DIR_EAST:
        if (actor->position.x >= map_width - 1) {
            return teleport(map, map_width, map_height, actor);
        }
        new_pos.x++;
        break;
    case DIR_SOUTH:
        if (actor->position.y >= map_height - 1) {
            return teleport(map, map_width, map_height, actor);
        }
        new_pos.y++;
        break;
    case DIR_WEST:
        if (actor->position.x <= 0) {
            return teleport(map, map_width, map_height, actor);
        }
        new_pos.x--;
        break;
    }

    switch (map[new_pos.y][new_pos.x]) {
    case TILE_OPEN:
        // good to go
        return new_pos;
    case TILE_WALL:
        // stop
        return actor->position;
    case TILE_EMPTY:
        // loop around
        return teleport(map, map_width, map_height, actor);
    default:
        fprintf(stderr, "unknown tile at (%d, %d)\n", new_pos.y, new_pos.x);
        exit(1);
    }
}

struct Actor solve(enum Tile (*map)[MAP_WIDTH], size_t map_width, size_t map_height,
        struct Instruction* instructions, size_t instructions_len, int32_t start_x)
{
    struct Actor actor = { .direction = DIR_EAST, .position = { .x = start_x, .y = 0 } };

    for (size_t i = 0; i < instructions_len; i++) {
        struct Instruction instr = instructions[i];
        switch (instr.type) {
        case INSTR_TURN:
            switch (instr.turn_direction) {
            case TURN_LEFT:
                if (actor.direction == DIR_NORTH) {
                    actor.direction = DIR_WEST;
                } else {
                    actor.direction--;
                }
                break;
            case TURN_RIGHT:
                if (actor.direction == DIR_WEST) {
                    actor.direction = DIR_NORTH;
                } else {
                    actor.direction++;
                }
                break;
            default:
                fprintf(stderr, "unknown turn direction %c\n", instr.turn_direction);
                exit(1);
            }
            break;
        case INSTR_MOVE:
            for (size_t d = 0; d < instr.distance; d++) {
                struct Position next_pos = step(map, map_width, map_height, &actor);
                if (next_pos.x == actor.position.x && next_pos.y == actor.position.y) {
                    // if we do not move, skip the rest of the instructions
                    break;
                } else {
                    actor.position = next_pos;
                }
            }
            break;
        default:
            fprintf(stderr, "unknown instruction\n");
            exit(1);
        }
    }

    return actor;
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

    char* buffer = calloc(BUFFER_SIZE, sizeof(*buffer));
    bool parsed_map = false;
    // allocate map
    enum Tile(*map)[MAP_WIDTH] = calloc(1, sizeof(enum Tile[MAP_HEIGHT][MAP_WIDTH]));
    struct Instruction* instructions = calloc(INSTRUCTIONS_SIZE, sizeof(*instructions));
    size_t instructions_len = 0;
    size_t map_height = 0;
    size_t map_width = 0;
    int32_t start_x = -1;
    while (fgets(buffer, BUFFER_SIZE, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        if (!parsed_map && buffer[0] == 0) {
            // encountered the empty line, this means we're done parsing the map
            parsed_map = true;
        } else if (parsed_map) {
            char* c = buffer;
            struct Instruction* instr = &instructions[instructions_len];
            instructions_len++;
            // parse the first integer
            instr->distance = atoi(c);
            instr->type = INSTR_MOVE;
            // go to the next turn indicator
            c += strcspn(c, "LR");
            for (;;) {
                // parse turn indicator
                struct Instruction* instr = &instructions[instructions_len];
                instructions_len++;
                instr->type = INSTR_TURN;
                if (*c != TURN_LEFT && *c != TURN_RIGHT) {
                    fprintf(stderr, "invalid turn directon '%c'", *c);
                    exit(1);
                }
                instr->turn_direction = *c;
                c++;

                // parse distance
                instr = &instructions[instructions_len];
                instructions_len++;
                instr->type = INSTR_MOVE;
                instr->distance = atoi(c);

                // update the char* to point to the next pair of instructions
                size_t offset = strcspn(c, "LR");
                if (offset == strlen(c)) {
                    // if there are no more LR characters left, we're done parsing
                    break;
                }
                c += offset;
            }
            break;
        } else {
            // continue parsing map
            char* c = buffer;
            size_t i = 0;
            for (; *c; c++, i++) {
                if (*c != ' ' && *c != '.' && *c != '#') {
                    fprintf(stderr, "unknown tile '%c' at x %zu, y %zu\n", *c, i, map_height);
                    exit(1);
                }

                switch (*c) {
                case ' ':
                    map[map_height][i] = TILE_EMPTY;
                    break;
                case '#':
                    map[map_height][i] = TILE_WALL;
                    break;
                case '.':
                    map[map_height][i] = TILE_OPEN;
                }
                if (map_height == 0 && start_x == -1 && *c == TILE_OPEN) {
                    start_x = i;
                }
            }

            // update the map dimensions
            if (i > map_width) {
                map_width = i;
            }
            map_height++;
        }
    }

#ifdef DEBUG
    for (size_t y = 0; y < map_height; y++) {
        for (size_t x = 0; x < map_width; x++) {
            switch (map[y][x]) {
            case TILE_OPEN:
                printf(".");
                break;
            case TILE_WALL:
                printf("#");
                break;
            case TILE_EMPTY:
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("\n");

    for (size_t i = 0; i < instructions_len; i++) {
        switch (instructions[i].type) {
        case INSTR_MOVE:
            printf("%d", instructions[i].distance);
            break;
        case INSTR_TURN:
            printf("%c", instructions[i].turn_direction);
            break;
        }
    }
    printf("\n");
    printf("starting at (%d, 0)\n", start_x);
#endif

    struct Actor actor = solve(map, map_width, map_height, instructions, instructions_len, start_x);
    printf("ended up @(%d, %d) facing ", actor.position.x, actor.position.y);
    int32_t dir_hash = 0;
    switch (actor.direction) {
    case DIR_NORTH:
        dir_hash = 3;
        printf("^\n");
        break;
    case DIR_WEST:
        dir_hash = 2;
        printf(">\n");
        break;
    case DIR_SOUTH:
        dir_hash = 1;
        printf("v\n");
        break;
    case DIR_EAST:
        dir_hash = 0;
        printf("<\n");
        break;
    }
    printf("password: %d\n", dir_hash + 1000 * (actor.position.y + 1) + 4 * (actor.position.x + 1));

    // free map and instructions
    free(buffer);
    free(map);
    free(instructions);

    fclose(fp);
    return 0;
}

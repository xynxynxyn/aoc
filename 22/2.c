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

int32_t normalize(int32_t x, int32_t cube_dim)
{
    return x % cube_dim;
}

int32_t mirror(int32_t x, int32_t cube_dim)
{
    return cube_dim - 1 - normalize(x, cube_dim);
}

struct Actor teleport(enum Tile (*map)[MAP_WIDTH], size_t map_width, size_t map_height, struct Actor* actor)
{
    // now we are on a cube, determine what side we are on and how we would turn
    int32_t x = actor->position.x;
    int32_t y = actor->position.y;
    int32_t cube_dim = map_width / 3;
    int32_t face;
    if (0 <= x && x < cube_dim) {
        if (cube_dim * 2 <= y && y < cube_dim * 3) {
            face = 4;
        } else if (cube_dim * 3 <= y && y < cube_dim * 4) {
            face = 6;
        } else {
            fprintf(stderr, "could not determine what face (%d, %d) is on\n", x, y);
            exit(1);
        }
    } else if (cube_dim <= x && x < cube_dim * 2) {
        // in the row with three faces
        if (0 <= y && y < cube_dim) {
            face = 1;
        } else if (cube_dim <= y && y < cube_dim * 2) {
            face = 3;
        } else if (cube_dim * 2 <= y && y < cube_dim * 3) {
            face = 5;
        } else {
            fprintf(stderr, "could not determine what face (%d, %d) is on\n", x, y);
            exit(1);
        }
    } else if (cube_dim * 2 <= x && x < map_width && 0 <= y && y < cube_dim) {
        face = 2;
    } else {
        fprintf(stderr, "could not determine what face (%d, %d) is on\n", x, y);
        exit(1);
    }

#ifdef DEBUG
    printf("teleport from f%d to f", face);
#endif

    struct Actor new_actor;
    switch (face) {
    case 1:
        switch (actor->direction) {
        case DIR_NORTH:
            // entering 6 from the left
            new_actor.position.y = cube_dim * 3 + normalize(actor->position.x, cube_dim);
            new_actor.position.x = 0;
            new_actor.direction = DIR_EAST;
#ifdef DEBUG
            printf("6\n");
#endif
            break;
        case DIR_WEST:
            // entering 4 from the left
            new_actor.position.y = cube_dim * 2 + mirror(actor->position.y, cube_dim);
            new_actor.position.x = 0;
            new_actor.direction = DIR_EAST;
#ifdef DEBUG
            printf("4\n");
#endif
            break;
        default:
            fprintf(stderr, "on face 1 but teleporting impossible (%d, %d)\n", x, y);
            exit(1);
        }
        break;
    case 2:
        switch (actor->direction) {
        case DIR_NORTH:
            // enter 6 from below
            new_actor.position.x = normalize(actor->position.x, cube_dim);
            new_actor.position.y = cube_dim * 4 - 1;
            new_actor.direction = DIR_NORTH;
#ifdef DEBUG
            printf("6\n");
#endif
            break;
        case DIR_EAST:
            // enter 5 from the right
            new_actor.position.x = cube_dim * 2 - 1;
            new_actor.position.y = cube_dim * 2 + mirror(actor->position.y, cube_dim);
            new_actor.direction = DIR_WEST;
#ifdef DEBUG
            printf("5\n");
#endif
            break;
        case DIR_SOUTH:
            // enter 3 from the right
            new_actor.position.x = cube_dim * 2 - 1;
            new_actor.position.y = cube_dim + normalize(actor->position.x, cube_dim);
            new_actor.direction = DIR_WEST;
#ifdef DEBUG
            printf("3\n");
#endif
            break;
        default:
            fprintf(stderr, "on face 2 but teleporting impossible (%d, %d)\n", x, y);
            exit(1);
        }
        break;
    case 3:
        switch (actor->direction) {
        case DIR_EAST:
            // enter 2 from below
            new_actor.position.y = cube_dim - 1;
            new_actor.position.x = cube_dim * 2 + normalize(actor->position.y, cube_dim);
            new_actor.direction = DIR_NORTH;
#ifdef DEBUG
            printf("2\n");
#endif
            break;
        case DIR_WEST:
            // enter 4 from above
            new_actor.position.y = cube_dim * 2;
            new_actor.position.x = normalize(actor->position.y, cube_dim);
            new_actor.direction = DIR_SOUTH;
#ifdef DEBUG
            printf("4\n");
#endif
            break;
        default:
            fprintf(stderr, "on face 3 but teleporting impossible (%d, %d)\n", x, y);
            exit(1);
        }
        break;
    case 4:
        switch (actor->direction) {
        case DIR_NORTH:
            // enter 3 from the left
            new_actor.position.x = cube_dim;
            new_actor.position.y = cube_dim + normalize(actor->position.x, cube_dim);
            new_actor.direction = DIR_EAST;
#ifdef DEBUG
            printf("3\n");
#endif
            break;
        case DIR_WEST:
            // enter 1 from the left
            new_actor.position.x = cube_dim;
            new_actor.position.y = mirror(actor->position.y, cube_dim);
            new_actor.direction = DIR_EAST;
#ifdef DEBUG
            printf("1\n");
#endif
            break;
        default:
            fprintf(stderr, "on face 3 but teleporting impossible (%d, %d)\n", x, y);
            exit(1);
        }
        break;
    case 5:
        switch (actor->direction) {
        case DIR_EAST:
            // enter 2 from the right
            new_actor.position.x = cube_dim * 3 - 1;
            new_actor.position.y = mirror(actor->position.y, cube_dim);
            new_actor.direction = DIR_WEST;
#ifdef DEBUG
            printf("2\n");
#endif
            break;
        case DIR_SOUTH:
            // enter 6 from the right
            new_actor.position.x = cube_dim - 1;
            new_actor.position.y = cube_dim * 3 + normalize(actor->position.x, cube_dim);
            new_actor.direction = DIR_WEST;
#ifdef DEBUG
            printf("6\n");
#endif
            break;
        default:
            fprintf(stderr, "on face 5 but teleporting impossible (%d, %d)\n", x, y);
            exit(1);
        }
        break;
    case 6:
        switch (actor->direction) {
        case DIR_EAST:
            // enter 5 from below
            new_actor.position.y = cube_dim * 3 - 1;
            new_actor.position.x = cube_dim + normalize(actor->position.y, cube_dim);
            new_actor.direction = DIR_NORTH;
#ifdef DEBUG
            printf("5\n");
#endif
            break;
        case DIR_SOUTH:
            // enter 2 from above
            new_actor.position.y = 0;
            new_actor.position.x = cube_dim * 2 + normalize(actor->position.x, cube_dim);
            new_actor.direction = DIR_SOUTH;
#ifdef DEBUG
            printf("2\n");
#endif
            break;
        case DIR_WEST:
            // enter 1 from above
            new_actor.position.y = 0;
            new_actor.position.x = cube_dim + normalize(actor->position.y, cube_dim);
            new_actor.direction = DIR_SOUTH;
#ifdef DEBUG
            printf("1\n");
#endif
            break;
        default:
            fprintf(stderr, "on face 6 but teleporting impossible (%d, %d)\n", x, y);
            exit(1);
        }
        break;
    }

    if (map[new_actor.position.y][new_actor.position.x] == TILE_WALL) {
        return *actor;
    } else {
        return new_actor;
    }
}

struct Actor step(enum Tile (*map)[MAP_WIDTH], size_t map_width, size_t map_height, struct Actor* actor)
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
    case TILE_OPEN: {
        // good to go
        struct Actor new_actor = { .direction = actor->direction, .position = new_pos };
        return new_actor;
    }
    case TILE_WALL: {
        // stop
        return *actor;
    }
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
                struct Actor next_state = step(map, map_width, map_height, &actor);
                if (next_state.position.x == actor.position.x && next_state.position.y == actor.position.y) {
                    // if we do not move, skip the rest of the instructions
#ifdef DEBUG
                    printf("stop @ (%d, %d)\n", actor.position.x, actor.position.y);
#endif
                    break;
                } else {
#ifdef DEBUG
                    printf("mv to  (%d, %d)\n", next_state.position.x, next_state.position.y);
#endif
                    actor = next_state;
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
            size_t offset = strcspn(c, "LR");
            if (offset == strlen(c)) {
                break;
            }
            c += offset;
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
        printf("<\n");
        break;
    case DIR_SOUTH:
        dir_hash = 1;
        printf("v\n");
        break;
    case DIR_EAST:
        dir_hash = 0;
        printf(">\n");
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

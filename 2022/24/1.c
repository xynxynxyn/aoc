#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define MAP_WIDTH 256
#define MAP_HEIGHT 256
#define QUEUE_SIZE 8128

struct Tile {
    bool bliz_n;
    bool bliz_e;
    bool bliz_s;
    bool bliz_w;
};

struct Map {
    struct Tile (*cur_map)[MAP_WIDTH];
    struct Tile (*tmp_map)[MAP_WIDTH];
};

void update(struct Map* map, size_t map_width, size_t map_height)
{
    // empty out the tmp_map
    for (size_t y = 0; y < map_height; y++) {
        for (size_t x = 0; x < map_width; x++) {
            map->tmp_map[y][x] = (struct Tile) { 0 };
        }
    }

    // go through every tile on the cur_map and set stuff on the tmp map
    for (size_t y = 0; y < map_height; y++) {
        for (size_t x = 0; x < map_width; x++) {
            if (map->cur_map[y][x].bliz_n) {
                // move blizzard north
                size_t next_y = y - 1;
                if (next_y == 0) {
                    // wrap around if at edge
                    next_y = map_height - 2;
                }
                map->tmp_map[next_y][x].bliz_n = true;
            }
            if (map->cur_map[y][x].bliz_e) {
                // move blizzard east
                size_t next_x = x + 1;
                if (next_x == map_width - 1) {
                    // wrap around if at edge
                    next_x = 1;
                }
                map->tmp_map[y][next_x].bliz_e = true;
            }
            if (map->cur_map[y][x].bliz_s) {
                // move blizzard south
                size_t next_y = y + 1;
                if (next_y == map_height - 1) {
                    // wrap around if at edge
                    next_y = 1;
                }
                map->tmp_map[next_y][x].bliz_s = true;
            }
            if (map->cur_map[y][x].bliz_w) {
                // move blizard west
                size_t next_x = x - 1;
                if (next_x == 0) {
                    // wrap around if at edge
                    next_x = map_width - 2;
                }
                map->tmp_map[y][next_x].bliz_w = true;
            }
        }
    }

    // swap the pointers to replace the cur_map with the fresh tmp_map
    struct Tile(*tmp)[MAP_WIDTH] = map->tmp_map;
    map->tmp_map = map->cur_map;
    map->cur_map = tmp;
}

void print_map(struct Map* map, size_t map_width, size_t map_height)
{
    for (size_t y = 0; y < map_height; y++) {
        for (size_t x = 0; x < map_width; x++) {
            if (y == 0) {
                if (x == 1) {
                    printf(".");
                } else {
                    printf("#");
                }
            } else if (y == map_height - 1) {
                if (x == map_width - 2) {
                    printf(".");
                } else {
                    printf("#");
                }
            } else if (x == 0 || x == map_width - 1) {
                printf("#");
            } else {
                struct Tile t = map->cur_map[y][x];
                printf("%d", t.bliz_n + t.bliz_e + t.bliz_s + t.bliz_w);
            }
        }
        printf("\n");
    }
}

// check if there are no blizzards in a given position
bool no_blizzard(struct Map* map, size_t x, size_t y)
{
    return map->cur_map[y][x].bliz_s + map->cur_map[y][x].bliz_w
            + map->cur_map[y][x].bliz_e + map->cur_map[y][x].bliz_n
            == 0;
}

struct Coord {
    size_t x;
    size_t y;
};

struct Route {
    struct Coord coords[3];
    size_t coords_len;
    size_t cur_goal;
};

void add_waypoint(struct Route* route, size_t x, size_t y)
{
    if (route->coords_len == 3) {
        fprintf(stderr, "only 3 waypoints per route\n");
        exit(1);
    }
    route->coords[route->coords_len] = (struct Coord) { x, y };
    route->coords_len++;
}

struct Coord current_waypoint(struct Route* route)
{
    return route->coords[route->cur_goal];
}

// tell the route object you arrived at a goal
// returns true if all goals have been reached
bool arrived_at_waypoint(struct Route* route)
{
    route->cur_goal++;
    return route->cur_goal == route->coords_len;
}

struct Player {
    size_t x;
    size_t y;
    int32_t minutes;
    struct Route route;
};

struct Queue {
    struct Player array[QUEUE_SIZE];
    size_t first;
    size_t length;
};

struct Player queue_pop(struct Queue* queue)
{
    if (queue->length == 0) {
        fprintf(stderr, "trying to pop from empty queue\n");
        exit(1);
    }

    struct Player p = queue->array[queue->first];
    queue->first = (queue->first + 1) % QUEUE_SIZE;
    queue->length--;
    return p;
}

bool queue_contains(struct Queue* queue, struct Player player)
{
    for (int32_t i = 0; i < queue->length; i++) {
        struct Player elem = queue->array[(queue->first + i) % QUEUE_SIZE];
        if (elem.minutes == player.minutes && elem.x == player.x && elem.y == player.y
                && elem.route.cur_goal == player.route.cur_goal) {
            return true;
        }
    }

    return false;
}

void queue_push(struct Queue* queue, struct Player player)
{
    if (queue->length == QUEUE_SIZE) {
        fprintf(stderr, "queue ran out of space\n");
        exit(1);
    }
    if (queue_contains(queue, player)) {
        // don't insert this state if we already explored it
        return;
    }
    queue->array[(queue->first + queue->length) % QUEUE_SIZE] = player;
    queue->length++;
}

int32_t bfs(struct Map* map, size_t map_width, size_t map_height,
        size_t start_x, size_t start_y, struct Route route)
{
    struct Queue queue = { 0 };
    struct Player init = { .x = start_x, .y = start_y, .minutes = 0, .route = route };
    queue_push(&queue, init);

    int32_t latest_time = -1;
    struct Player current;
    while (queue.length != 0) {
        current = queue_pop(&queue);

        // check if the current player has reached their waypoint
        if (current.x == current_waypoint(&current.route).x
                && current.y == current_waypoint(&current.route).y) {
            // update the waypoints for this player
            if (arrived_at_waypoint(&current.route)) {
                // all goals for this player completed
                return current.minutes;
            }
        }

        // update the map whenever we reach the next level of bfs
        if (current.minutes > latest_time) {
            update(map, map_width, map_height);
            latest_time = current.minutes;
#ifdef DEBUG
            printf("\n-- updated map %3d min --\n", latest_time + 1);
            print_map(map, map_width, map_height);
            printf("\n");
#endif
        }

        // go over all the possible moves
        // check if we can stay in the same spot
        struct Player next_state = current;
        next_state.minutes++;

        // check north (extra stuff because of going back to starting position)
        if ((current.x == 1 && current.y > 0) || current.y > 1) {
            if (no_blizzard(map, current.x, current.y - 1)) {
                next_state.x = current.x;
                next_state.y = current.y - 1;
                queue_push(&queue, next_state);
            }
        }

        // check south (extra stuff to go down to the goal position)
        if ((current.x == map_width - 2 && current.y < map_height - 1) || current.y < map_height - 2) {
            if (no_blizzard(map, current.x, current.y + 1)) {
                next_state.x = current.x;
                next_state.y = current.y + 1;
                queue_push(&queue, next_state);
            }
        }

        // check east
        if (current.y != 0 && current.y != map_height - 1 && current.x < map_width - 2) {
            if (no_blizzard(map, current.x + 1, current.y)) {
                next_state.x = current.x + 1;
                next_state.y = current.y;
                queue_push(&queue, next_state);
            }
        }

        // check west
        if (current.y != 0 && current.y != map_height - 1 && current.x > 1) {
            if (no_blizzard(map, current.x - 1, current.y)) {
                next_state.x = current.x - 1;
                next_state.y = current.y;
                queue_push(&queue, next_state);
            }
        }

        if (no_blizzard(map, current.x, current.y)) {
            next_state.x = current.x;
            next_state.y = current.y;
            queue_push(&queue, next_state);
        }
    }

    return -1;
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
    struct Map map;
    map.cur_map = calloc(1, sizeof(struct Tile[MAP_HEIGHT][MAP_WIDTH]));
    map.tmp_map = calloc(1, sizeof(struct Tile[MAP_HEIGHT][MAP_WIDTH]));
    size_t map_height = 0;
    size_t map_width = 0;
    while (fgets(buffer, BUFFER_SIZE, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        char* c = buffer;
        size_t x = 0;
        while (*c) {
            switch (*c) {
            case '>':
                map.cur_map[map_height][x].bliz_e = true;
                break;
            case '^':
                map.cur_map[map_height][x].bliz_n = true;
                break;
            case 'v':
                map.cur_map[map_height][x].bliz_s = true;
                break;
            case '<':
                map.cur_map[map_height][x].bliz_w = true;
                break;
            }
            c++;
            x++;
        }
        map_width = x;
        map_height++;
    }

    size_t start_x = 1;
    size_t start_y = 0;
    size_t goal_x = map_width - 2;
    size_t goal_y = map_height - 1;

    struct Route route = { 0 };
    add_waypoint(&route, map_width - 2, map_height - 1);
    // leave these two lines out for part 1
    add_waypoint(&route, 1, 0);
    add_waypoint(&route, map_width - 2, map_height - 1);

    int32_t minutes = bfs(&map, map_width, map_height, 1, 0, route);
    printf("%d\n", minutes);

    free(map.cur_map);
    free(map.tmp_map);
    fclose(fp);
    return 0;
}

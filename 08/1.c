#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAP_HEIGHT 256
#define MAP_WIDTH 256

bool tree_is_visible(char map[MAP_HEIGHT][MAP_WIDTH], size_t width, size_t height, size_t tree_x, size_t tree_y)
{
    // check edge
    if (tree_x == 0 || tree_y == 0 || tree_x == width - 1 || tree_y == height - 1) {
        return true;
    }

    char tree = map[tree_y][tree_x];

    bool north = true;
    bool east = true;
    bool south = true;
    bool west = true;

    // north
    for (size_t i = 0; i < tree_y; i++) {
        if (map[i][tree_x] >= tree) {
            north = false;
            break;
        }
    }
    // east
    for (size_t i = tree_x + 1; i < height; i++) {
        if (map[tree_y][i] >= tree) {
            east = false;
            break;
        }
    }
    // south
    for (size_t i = tree_y + 1; i < height; i++) {
        if (map[i][tree_x] >= tree) {
            south = false;
            break;
        }
    }
    // west
    for (size_t i = 0; i < tree_x; i++) {
        if (map[tree_y][i] >= tree) {
            west = false;
            break;
        }
    }

    return north || east || south || west;
}

int32_t scenic_score(char map[MAP_HEIGHT][MAP_WIDTH], size_t width, size_t height, size_t tree_x, size_t tree_y)
{
    char tree = map[tree_y][tree_x];
    int32_t north = 0;
    int32_t south = 0;
    int32_t west = 0;
    int32_t east = 0;

    // north
    for (int32_t i = tree_y - 1; i >= 0; i--) {
        if (map[i][tree_x] >= tree) {
            north++;
            break;
        }
        north++;
    }

    // south
    for (int32_t i = tree_y + 1; i < height; i++) {
        if (map[i][tree_x] >= tree) {
            south++;
            break;
        }
        south++;
    }

    // west
    for (int32_t i = tree_x + 1; i < width; i++) {
        if (map[tree_y][i] >= tree) {
            west++;
            break;
        }
        west++;
    }

    // east
    for (int32_t i = tree_x - 1; i >= 0; i--) {
        if (map[tree_y][i] >= tree) {
            east++;
            break;
        }
        east++;
    }

    return north * west * south * east;
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
    size_t width = 0;
    size_t height = 0;

    const size_t buffer_size = 256;
    char buffer[buffer_size];
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
        strncpy(map[height], buffer, MAP_WIDTH);
        height++;
    }
    width = strlen(map[0]);

    uint32_t visible_trees = 0;
    int32_t max_scenic_score = 0;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            int32_t score = scenic_score(map, width, height, x, y);
            if (score > max_scenic_score) {
                max_scenic_score = score;
            }
            if (tree_is_visible(map, width, height, x, y)) {
                visible_trees++;
                printf("%c", map[y][x]);
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }

    printf("visible trees: %d\n", visible_trees);
    printf("max scenic score: %d\n", max_scenic_score);

    fclose(fp);
    return 0;
}

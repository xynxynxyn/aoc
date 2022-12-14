#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct Point {
    int32_t x;
    int32_t y;
};

struct Line {
    struct Point src;
    struct Point end;
    bool horizontal;
};

struct Waterfall {
    struct Line lines[4096];
    size_t lines_len;
};

void add_line(struct Waterfall* waterfall, struct Point src, struct Point end)
{
    struct Line line = { src, end, src.y == end.y };
    waterfall->lines[waterfall->lines_len] = line;
    waterfall->lines_len++;
}

bool collides(struct Waterfall* waterfall, int32_t x, int32_t y)
{
    for (size_t i = 0; i < waterfall->lines_len; i++) {
        struct Line rocks = waterfall->lines[i];
        if (rocks.horizontal) {
            if (rocks.src.y == y
                && ((rocks.src.x <= x && x <= rocks.end.x) || (rocks.end.x <= x && x <= rocks.src.x))) {
                return true;
            }
        } else {
            if (rocks.src.x == x
                && ((rocks.src.y <= y && y <= rocks.end.y) || (rocks.end.y <= y && y <= rocks.src.y))) {
                return true;
            }
        }
    }

    return false;
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

    const size_t buffer_size = 1024;
    char buffer[buffer_size];
    struct Waterfall waterfall = { 0 };
    // the bottom of the rocks
    int32_t largest_y = 0;
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;

        struct Point points[256] = { 0 };
        size_t points_len = 0;

        // scan the first point
        if (sscanf(buffer, "%d,%d", &points[points_len].x, &points[points_len].y) != 2) {
            fprintf(stderr, "failed to parse line %s\n", buffer);
        }
        points_len++;

        // scan the rest by finding the -
        size_t input_len = strlen(buffer);
        for (;;) {
            size_t index = strcspn(buffer, "-");
            if (index == input_len) {
                // break if there are no ->'s left
                break;
            }

            buffer[index] = ' ';
            buffer[index + 1] = ' ';
            if (sscanf(buffer + index + 3, "%d,%d", &points[points_len].x, &points[points_len].y) != 2) {
                fprintf(stderr, "failed to parse line %s\n", buffer + index + 3);
            }
            points_len++;
        }

        for (size_t i = 0; i < points_len - 1; i++) {
            if (points[i].y > largest_y) {
                largest_y = points[i].y;
            }
            if (points[i + 1].y > largest_y) {
                largest_y = points[i + 1].y;
            }
            add_line(&waterfall, points[i], points[i + 1]);
        }
    }

    // Drop down sand
    size_t dropped = 0;

    for (;;) {
        struct Point sand = { 500, 0 };

        for (;;) {
            if (!collides(&waterfall, sand.x, sand.y + 1)) {
                // check below
                sand.y += 1;
            } else if (!collides(&waterfall, sand.x - 1, sand.y + 1)) {
                // below + left
                sand.x -= 1;
                sand.y += 1;
            } else if (!collides(&waterfall, sand.x + 1, sand.y + 1)) {
                // below + right
                sand.x += 1;
                sand.y += 1;
            } else {
                // at rest
                dropped++;
                // add single point to waterfall
                add_line(&waterfall, sand, sand);
                break;
            }

            // check if we fall below the threshold
            // if yes, stop the count
            if (sand.y >= largest_y) {
                goto abyss;
            }
        }
    }

abyss:
    printf("dropped %zu\n", dropped);

    fclose(fp);
    return 0;
}

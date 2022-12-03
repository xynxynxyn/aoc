#include <stdint.h>
#include <stdio.h>
#include <string.h>

int32_t item_priority(char item)
{
    if (item >= 97 && item <= 122) {
        // lowercase
        return item - 96;
    } else if (item >= 65 && item <= 90) {
        // uppercase
        return item - 38;
    } else {
        fprintf(stderr, "invalid item %c\n", item);
        return 0;
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
    uint32_t priority_sum = 0;
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        size_t backpack_size = strlen(buffer) / 2;

        for (size_t b1_offset = 0; b1_offset < backpack_size; b1_offset++) {
            // iterate through chars and check if second backpack contains that char
            // have to use indices since the first backpack is not null terminated
            if (strchr(buffer + backpack_size, buffer[b1_offset]) != NULL) {
                priority_sum += item_priority(buffer[b1_offset]);
                break;
            }
        }
    }

    printf("sum: %d\n", priority_sum);

    return 0;
}

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
    char elf1[buffer_size];
    char elf2[buffer_size];
    char elf3[buffer_size];
    uint32_t badge_sum = 0;

    // take 3 elfs at a time
    while (fgets(elf1, buffer_size, fp)
        && fgets(elf2, buffer_size, fp)
        && fgets(elf3, buffer_size, fp)) {
        // replace newline with null terminator
        elf1[strcspn(elf1, "\n")] = 0;
        elf2[strcspn(elf2, "\n")] = 0;
        elf3[strcspn(elf3, "\n")] = 0;

        size_t elf1_len = strlen(elf1);

        for (char* item = elf1; *item; item++) {
            // iterate through items of the first elf and check if the other
            // two elfs also have that item
            if ((strchr(elf2, *item) != NULL) && strchr(elf3, *item)) {
                // found badge
                badge_sum += item_priority(*item);
                break;
            }
        }
    }

    printf("sum: %d\n", badge_sum);

    return 0;
}

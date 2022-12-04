#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct Interval {
    uint32_t lower;
    uint32_t upper;
};

bool contains(struct Interval* fst, struct Interval* snd)
{
    return fst->lower <= snd->lower && fst->upper >= snd->upper;
}

bool overlaps(struct Interval* fst, struct Interval* snd)
{
    return (fst->upper >= snd->lower && fst->lower <= snd->lower)
        || (snd->upper >= fst->lower && snd->lower <= fst->upper);
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
    uint32_t contained_ct = 0;
    uint32_t overlap_ct = 0;
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
        struct Interval elf1;
        struct Interval elf2;
        if (sscanf(buffer, "%d-%d,%d-%d", &elf1.lower, &elf1.upper, &elf2.lower, &elf2.upper) != 4) {
            fprintf(stderr, "invalid format %s\n", buffer);
        }

        if (contains(&elf1, &elf2) || contains(&elf2, &elf1)) {
            contained_ct++;
        }

        if (overlaps(&elf1, &elf2)) {
            overlap_ct++;
        }
    }

    printf("%d\n%d\n", contained_ct, overlap_ct);

    fclose(fp);
    return 0;
}

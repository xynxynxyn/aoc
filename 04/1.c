#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

bool contains(uint32_t fst_l, uint32_t fst_u, uint32_t snd_l, uint32_t snd_u)
{
    return fst_l <= snd_l && fst_u >= snd_u;
}

bool overlaps(uint32_t fst_l, uint32_t fst_u, uint32_t snd_l, uint32_t snd_u) {
    return (fst_u >= snd_l && fst_l <= snd_l) || (snd_u >= fst_l && snd_l <= fst_l);
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
        uint32_t elf1_l;
        uint32_t elf1_u;
        uint32_t elf2_l;
        uint32_t elf2_u;
        sscanf(buffer, "%d-%d,%d-%d", &elf1_l, &elf1_u, &elf2_l, &elf2_u);

        if (contains(elf1_l, elf1_u, elf2_l, elf2_u) || contains(elf2_l, elf2_u, elf1_l, elf1_u)) {
            contained_ct++;
        }

        if (overlaps(elf1_l, elf1_u, elf2_l, elf2_u)) {
            overlap_ct++;
        }
    }

    printf("%d\n%d\n", contained_ct, overlap_ct);

    fclose(fp);
    return 0;
}

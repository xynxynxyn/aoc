#include <stdint.h>
#include <stdio.h>

uint32_t* min_element(uint32_t* elements, size_t n);
uint32_t* max_element(uint32_t* elements, size_t n);

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
    uint32_t current_calories = 0;
    uint32_t top[3] = { 0 };

    while (fgets(buffer, buffer_size, fp)) {
        uint32_t calories = 0;
        if (1 != sscanf(buffer, "%d", &calories)) {
            // empty line
            uint32_t* lowest = min_element(top, 3);
            if (current_calories > *lowest) {
                *lowest = current_calories;
            }
            current_calories = 0;
        } else {
            current_calories += calories;
        }
    }

    // last elf
    uint32_t* lowest = min_element(top, 3);
    if (current_calories > *lowest) {
        *lowest = current_calories;
    }

    printf("top 1 cal: %d\n", *max_element(top, 3));
    printf("top 3 sum: %d\n", top[0] + top[1] + top[2]);

    return 0;
}

uint32_t* min_element(uint32_t* elements, size_t n)
{
    if (n == 0) {
        return NULL;
    }

    uint32_t* lowest = elements;
    for (size_t i = 1; i < n; i++) {
        if (elements[i] < *lowest) {
            lowest = &elements[i];
        }
    }

    return lowest;
}

uint32_t* max_element(uint32_t* elements, size_t n)
{
    if (n == 0) {
        return NULL;
    }

    uint32_t* max = elements;
    for (size_t i = 1; i < n; i++) {
        if (elements[i] > *max) {
            max = &elements[i];
        }
    }

    return max;
}

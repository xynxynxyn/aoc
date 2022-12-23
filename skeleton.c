#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

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
    while (fgets(buffer, BUFFER_SIZE, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
    }

    fclose(fp);
    return 0;
}

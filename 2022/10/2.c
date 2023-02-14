#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CPU {
    int32_t reg_x;
    int32_t cycle;
};

void execute(struct CPU* cpu)
{
    cpu->cycle++;
    int32_t pixel = (cpu->cycle - 1) % 40;
    if (cpu->reg_x - 1 <= pixel && pixel <= cpu->reg_x + 1) {
        printf("#");
    } else {
        printf(".");
    }

    if (pixel > 0 && (pixel + 1) % 40 == 0) {
        printf("\n");
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
    struct CPU cpu = { 1, 0 };
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;

        int32_t addx;
        if (strcmp("noop", buffer) == 0) {
            // noop takes 1 cycle and does nothing
            execute(&cpu);
        } else if (sscanf(buffer, "addx %d", &addx) == 1) {
            // addx takes two cycles and updates reg_x
            execute(&cpu);
            execute(&cpu);
            cpu.reg_x += addx;
        } else {
            fprintf(stderr, "unknown instruction %s\n", buffer);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

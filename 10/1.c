#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CPU {
    int32_t reg_x;
    int32_t cycle;
};

void execute(struct CPU* cpu, int32_t* signal_sum)
{
    cpu->cycle++;
    // calculate signal strength
    int32_t cycle = cpu->cycle;
    if (cycle == 20 || cycle == 60 || cycle == 100
        || cycle == 140 || cycle == 180 || cycle == 220) {
        int32_t signal_strength = cpu->reg_x * cycle;
        *signal_sum += signal_strength;
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
    int32_t signal_sum = 0;
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;

        int32_t addx;
        if (strcmp("noop", buffer) == 0) {
            // noop takes 1 cycle and does nothing
            execute(&cpu, &signal_sum);
        } else if (sscanf(buffer, "addx %d", &addx) == 1) {
            // addx takes two cycles and updates reg_x
            execute(&cpu, &signal_sum);
            execute(&cpu, &signal_sum);
            cpu.reg_x += addx;
        } else {
            fprintf(stderr, "unknown instruction %s\n", buffer);
            return 1;
        }
    }

    printf("%d\n", signal_sum);

    fclose(fp);
    return 0;
}

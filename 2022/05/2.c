#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Stack {
    char items[64];
    size_t size;
};

char pop(struct Stack* stack)
{
    if (stack->size == 0) {
        return 0;
    }

    stack->size--;
    return stack->items[stack->size];
}

void push(struct Stack* stack, char item)
{
    stack->items[stack->size] = item;
    stack->size++;
}

char peek(struct Stack* stack)
{
    if (stack->size == 0) {
        return 0;
    }
    return stack->items[stack->size - 1];
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

    const size_t buffer_size = 64;
    char buffer[buffer_size];
    char crates[64][buffer_size];
    size_t line_number = 0;
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) == 0) {
            // if we reach an empty line we parsed all the crates
            break;
        }
        strncpy(crates[line_number], buffer, buffer_size);
        line_number++;
    }

    size_t number_of_stacks = strlen((crates[line_number - 1]) + 1) / 4 + 1;
    struct Stack* stacks = calloc(sizeof(*stacks), number_of_stacks);

    // construct the stacks
    for (int32_t row = line_number - 2; row >= 0; row--) {
        for (int32_t column = 0; column < number_of_stacks; column++) {
            char item = crates[row][1 + column * 4];
            if (item == ' ') {
                continue;
            }
            push(&stacks[column], item);
        }
    }

    while (fgets(buffer, buffer_size, fp)) {
        // crate moves
        int32_t amount, source, target;

        sscanf(buffer, "move %d from %d to %d", &amount, &source, &target);
        source--;
        target--;

        char items[amount];
        for (int32_t i = 0; i < amount; i++) {
            items[i] = pop(&stacks[source]);
        }

        for (int32_t i = amount - 1; i >= 0; i--) {
            push(&stacks[target], items[i]);
        }
    }

    for (int32_t i = 0; i < number_of_stacks; i++) {
        printf("%c", peek(&stacks[i]));
    }
    printf("\n");

    free(stacks);
    fclose(fp);
    return 0;
}

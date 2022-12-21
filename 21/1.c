#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 32

enum MonkeyType { MONKEY_NUMBER,
    MONKEY_MULTIPLY,
    MONKEY_ADD,
    MONKEY_SUBSTRACT,
    MONKEY_DIVIDE };

struct Monkey {
    char name[5];
    enum MonkeyType type;
    int64_t result;
    bool done;
    struct Monkey* lhs;
    struct Monkey* rhs;
};

void monkey_compute(struct Monkey* monkey)
{
    if (monkey->done) {
        return;
    }

    if (!monkey->lhs->done) {
        monkey_compute(monkey->lhs);
    }
    if (!monkey->rhs->done) {
        monkey_compute(monkey->rhs);
    }

    switch (monkey->type) {
    case MONKEY_ADD:
        monkey->result = monkey->lhs->result + monkey->rhs->result;
        break;
    case MONKEY_DIVIDE:
        monkey->result = monkey->lhs->result / monkey->rhs->result;
        break;
    case MONKEY_MULTIPLY:
        monkey->result = monkey->lhs->result * monkey->rhs->result;
        break;
    case MONKEY_SUBSTRACT:
        monkey->result = monkey->lhs->result - monkey->rhs->result;
        break;
    default:
        fprintf(stderr, "unknown operand for monkey %s\n", monkey->name);
        exit(1);
    }

    monkey->done = true;
}

struct Monkey* find_monkey_with_name(struct Monkey* monkeys, size_t monkeys_len, char* name)
{
    for (size_t i = 0; i < monkeys_len; i++) {
        if (strncmp(monkeys[i].name, name, 4) == 0) {
            return &monkeys[i];
        }
    }
    return NULL;
}

size_t number_of_lines(FILE* f)
{
    fseek(f, 0, SEEK_SET);
    size_t lines = 0;

    char c = getc(f);
    for (; c != EOF; c = getc(f)) {
        if (c == '\n') {
            lines++;
        }
    }

    // reset the offset again
    fseek(f, 0, SEEK_SET);

    return lines;
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

    size_t monkeys_len = number_of_lines(fp);
    char buffer[monkeys_len][BUFFER_SIZE];
    struct Monkey monkeys[monkeys_len];
    size_t monkey_index = 0;
    while (fgets(buffer[monkey_index], BUFFER_SIZE, fp)) {
        // replace newline with null terminator
        buffer[monkey_index][strcspn(buffer[monkey_index], "\n")] = 0;
        // sscanf stuff

        char operand;
        char lhs[5];
        char rhs[5];
        struct Monkey* monkey = &monkeys[monkey_index];
        if (sscanf(buffer[monkey_index], "%4s: %ld", monkey->name, &monkey->result) == 2) {
            monkey->name[4] = 0;
            // got a number monkey
            monkey->type = MONKEY_NUMBER;
            monkey->done = true;
        } else if (sscanf(buffer[monkey_index], "%4s: %4s %c %4s", monkey->name, lhs, &operand, rhs) == 4) {
            // find the other monkeys in the computation already if we can
            monkey->lhs = find_monkey_with_name(monkeys, monkey_index, lhs);
            monkey->rhs = find_monkey_with_name(monkeys, monkey_index, rhs);

            // got a computation monkey
            monkey->done = false;
            switch (operand) {
            case '*':
                monkey->type = MONKEY_MULTIPLY;
                break;
            case '+':
                monkey->type = MONKEY_ADD;
                break;
            case '-':
                monkey->type = MONKEY_SUBSTRACT;
                break;
            case '/':
                monkey->type = MONKEY_DIVIDE;
                break;
            default:
                fprintf(stderr, "unknown monkey type '%s'\n", buffer[monkey_index]);
                break;
            }
        }
        monkey_index++;
    }

    // assign the remaining monkey dependencies
    for (size_t i = 0; i < monkeys_len; i++) {
        struct Monkey* monkey = &monkeys[i];
        if (monkey->type == MONKEY_SUBSTRACT || monkey->type == MONKEY_MULTIPLY
            || monkey->type == MONKEY_ADD || monkey->type == MONKEY_DIVIDE) {
            char name[5], lhs[5], rhs[5];
            char operand;
            if (sscanf(buffer[i], "%4s: %4s %c %4s", name, lhs, &operand, rhs) != 4) {
                fprintf(stderr, "could not parse line '%s'\n", buffer[i]);
                exit(1);
            }
            if (monkey->lhs == NULL) {
                monkey->lhs = find_monkey_with_name(monkeys, monkey_index, lhs);
            }
            if (monkey->rhs == NULL) {
                monkey->rhs = find_monkey_with_name(monkeys, monkey_index, rhs);
            }

            if (monkey->lhs == NULL || monkey->rhs == NULL) {
                printf("could not resolve monkey with names %s and %s\n", lhs, rhs);
            }
        }
    }

    struct Monkey* root_monkey = NULL;

    for (size_t i = 0; i < monkeys_len; i++) {
        if (strncmp(monkeys[i].name, "root", 4) == 0) {
            // found root monkey
            root_monkey = &monkeys[i];
            break;
        }
    }

    if (root_monkey == NULL) {
        fprintf(stderr, "could not find root monkey\n");
        exit(1);
    }

    monkey_compute(root_monkey);
    printf("root: %ld\n", root_monkey->result);

    fclose(fp);
    return 0;
}

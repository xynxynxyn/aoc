#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct Item {
    int64_t worry_lvl;
};

struct Monkey {
    struct Item* items[256];
    size_t items_len;
    int64_t constant;
    int64_t test_divisor;
    size_t next_if_true;
    size_t next_if_false;
    int64_t inspect_count;
    void (*update)(struct Item*, int64_t);
};

void monkey_receive_item(struct Monkey* monkey, struct Item* item, int64_t modulo)
{
    if (modulo != 0) {
        item->worry_lvl %= modulo;
    }
    monkey->items[monkey->items_len] = item;
    monkey->items_len++;
}

void monkey_empty_items(struct Monkey* monkey)
{
    for (size_t i = 0; i < monkey->items_len; i++) {
        monkey->items[i] = NULL;
    }
    monkey->items_len = 0;
}

void square(struct Item* item, int64_t constant)
{
    item->worry_lvl *= item->worry_lvl;
}

void add(struct Item* item, int64_t constant)
{
    item->worry_lvl += constant;
}

void mul(struct Item* item, int64_t constant)
{
    item->worry_lvl *= constant;
}

void show_monkeys(struct Monkey* monkeys, size_t monkeys_len)
{
    for (size_t m = 0; m < monkeys_len; m++) {
        printf("Monkey %zu inspected %ld times\n", m, monkeys[m].inspect_count);
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

    struct Item items[256] = { 0 };
    size_t items_len = 0;

    struct Monkey monkeys[32] = { 0 };
    size_t monkey_index = 0;
    size_t monkeys_len = 0;
    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
        int64_t constant;
        size_t next_monkey_index;
        if (sscanf(buffer, "Monkey %zu", &monkey_index) == 1) {
            monkeys_len++;
        } else if (strncmp(buffer, "  Starting items: ", 18) == 0) {
            // parsing items
            size_t number_of_items = (strlen(buffer) - 18 + 2) / 4;
            for (size_t i = 0; i < number_of_items; i++) {
                int64_t item_id;
                if (sscanf(buffer + 18 + i * 4, "%2ld", &item_id) != 1) {
                    fprintf(stderr, "could not parse items in %s\n", buffer);
                    return 1;
                }

                items[items_len].worry_lvl = item_id;
                monkey_receive_item(&monkeys[monkey_index], &items[items_len], 0);
                items_len++;
            }
        } else if (strcmp(buffer, "  Operation: new = old * old") == 0) {
            // set monkey operation method to square
            monkeys[monkey_index].update = square;
        } else if (sscanf(buffer, "  Operation: new = old * %ld", &constant) == 1) {
            // set monkey operation to multiply
            monkeys[monkey_index].update = mul;
            monkeys[monkey_index].constant = constant;
        } else if (sscanf(buffer, "  Operation: new = old + %ld", &constant) == 1) {
            // set monkey operation to add
            monkeys[monkey_index].update = add;
            monkeys[monkey_index].constant = constant;
        } else if (sscanf(buffer, "  Test: divisible by %ld", &constant) == 1) {
            monkeys[monkey_index].test_divisor = constant;
        } else if (sscanf(buffer, "    If true: throw to monkey %zu", &next_monkey_index) == 1) {
            monkeys[monkey_index].next_if_true = next_monkey_index;
        } else if (sscanf(buffer, "    If false: throw to monkey %zu", &next_monkey_index) == 1) {
            monkeys[monkey_index].next_if_false = next_monkey_index;
        } else {
            continue;
        }
    }

    int64_t modulo = 1;

    for (size_t m = 0; m < monkeys_len; m++) {
        modulo *= monkeys[m].test_divisor;
    }

    size_t round = 0;
    printf("Initial state\n");
    show_monkeys(monkeys, monkeys_len);

    // Part 1
    // while (round < 20) {
    while (round < 10000) {
        for (size_t m = 0; m < monkeys_len; m++) {
            struct Monkey* monkey = &monkeys[m];
            for (size_t i = 0; i < monkey->items_len; i++) {
                monkey->inspect_count++;
                struct Item* item = monkey->items[i];
                monkey->update(item, monkey->constant);
                // Part 1
                // item->worry_lvl /= 3;

                if (item->worry_lvl % monkey->test_divisor == 0) {
                    monkey_receive_item(&monkeys[monkey->next_if_true], item, modulo);
                } else {
                    monkey_receive_item(&monkeys[monkey->next_if_false], item, modulo);
                }
            }

            monkey_empty_items(monkey);
        }

        round++;
        if (round <= 20 || round % 2000 == 0) {
            printf("After round %zu\n", round);
            show_monkeys(monkeys, monkeys_len);
        }
    }

    fclose(fp);
    return 0;
}

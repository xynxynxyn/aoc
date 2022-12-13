#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum ELEM_TYPE { ELEM_TYPE_LIST,
    ELEM_TYPE_INTEGER };

struct Elem {
    struct List* list;
    int32_t integer;
    enum ELEM_TYPE type;
};

struct List {
    struct Elem* elems[256];
    size_t elems_size;
};

int32_t elem_cmp(const struct Elem* left, const struct Elem* right)
{
    if (left->type == ELEM_TYPE_INTEGER && right->type == ELEM_TYPE_INTEGER) {
        if (left->integer == right->integer) {
            return 0;
        } else if (left->integer < right->integer) {
            return -1;
        } else {
            return 1;
        }
    } else if (left->type == ELEM_TYPE_INTEGER) {
        // if the right list is empty this is false
        if (right->list->elems_size == 0) {
            return 1;
        }
        // Otherwise compare against first element of the right list
        int32_t cmp = elem_cmp(left, right->list->elems[0]);
        if (cmp == 0 && right->list->elems_size == 1) {
            return 0;
        } else if (cmp > 0) {
            return 1;
        } else {
            return -1;
        }
    } else if (right->type == ELEM_TYPE_INTEGER) {
        // Left is a list and right is not
        if (left->list->elems_size == 0) {
            return -1;
        }
        // Compare first elements
        int32_t cmp = elem_cmp(left->list->elems[0], right);
        if (cmp == 0) {
            if (left->list->elems_size == 1) {
                return 0;
            } else {
                return 1;
            }
        } else if (cmp > 0) {
            return 1;
        } else {
            return -1;
        }
    } else {
        // both are lists
        size_t left_len = left->list->elems_size;
        size_t right_len = right->list->elems_size;
        size_t smallest = left_len < right_len ? left_len : right_len;
        for (size_t i = 0; i < smallest; i++) {
            int32_t cmp = elem_cmp(left->list->elems[i], right->list->elems[i]);
            if (cmp == 0) {
                continue;
            } else {
                return cmp;
            }
        }

        if (left_len > right_len) {
            return 1;
        } else if (left_len == right_len) {
            return 0;
        } else {
            return -1;
        }
    }
}

void elem_free(struct Elem* elem)
{
    if (elem->type == ELEM_TYPE_LIST) {
        for (size_t i = 0; i < elem->list->elems_size; i++) {
            elem_free(elem->list->elems[i]);
        }
        free(elem->list);
    }
    free(elem);
}

struct Elem* parse_elem(char**);

void list_insert(struct List* list, struct Elem* elem)
{
    list->elems[list->elems_size] = elem;
    list->elems_size++;
}

struct List* parse_list(char** str)
{
    struct List* list = malloc(sizeof(*list));
    list->elems_size = 0;
    if (strncmp("[]", *str, 2) == 0) {
        // Empty list, return 0
        list->elems_size = 0;
        *str += 2;
        return list;
    }

    // skip [
    (*str)++;
    while (**str != ']') {
        struct Elem* elem = parse_elem(str);
        list_insert(list, elem);
    }

    (*str)++;

    return list;
}

struct Elem* parse_elem(char** str)
{
    struct Elem* elem = malloc(sizeof(*elem));
    int32_t integer;
    if (**str == '[') {
        // parse a list
        elem->type = ELEM_TYPE_LIST;
        elem->list = parse_list(str);
    } else if (sscanf(*str, "%d", &integer) == 1) {
        // got an integer
        elem->type = ELEM_TYPE_INTEGER;
        elem->integer = integer;
        size_t integer_len = strcspn(*str, ",]");
        *str += integer_len;
        // skip the comma
    }
    if (**str == ',') {
        (*str)++;
    }

    return elem;
}

struct Elem* elem_parse(char* str)
{
    char* input = str;
    return parse_elem(&input);
}

void print_elem(struct Elem* elem)
{
    switch (elem->type) {
    case ELEM_TYPE_INTEGER:
        printf("%d", elem->integer);
        break;
    case ELEM_TYPE_LIST:
        printf("[");
        if (elem->list->elems_size > 0) {
            print_elem(elem->list->elems[0]);
        }
        for (size_t i = 1; i < elem->list->elems_size; i++) {
            printf(",");
            print_elem(elem->list->elems[i]);
        }
        printf("]");
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
    struct Elem* signals[256];
    char* buffer = malloc(buffer_size);
    size_t signals_len = 0;

    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(buffer) == 0) {
            // skip if empty line
            continue;
        }
        struct Elem* elem = elem_parse(buffer);
        signals[signals_len] = elem;
        signals_len++;
    }

    free(buffer);

    // sort elems
    for (size_t i = 0; i < signals_len - 1; i++) {
        for (size_t j = 0; j < signals_len - i - 1; j++) {
            if (elem_cmp(signals[j], signals[j + 1]) > 0) {
                struct Elem* tmp = signals[j];
                signals[j] = signals[j + 1];
                signals[j + 1] = tmp;
            }
        }
    }

    // create divider signals
    struct Elem* divider1 = elem_parse("[[2]]");
    struct Elem* divider2 = elem_parse("[[6]]");

    int32_t part2 = 1;
    for (size_t i = 0; i < signals_len; i++) {
        printf("%3d ", i + 1);
        print_elem(signals[i]);
        printf("\n");
        if (elem_cmp(divider1, signals[i]) == 0 || elem_cmp(divider2, signals[i]) == 0) {
            part2 *= (i + 1);
        }
        elem_free(signals[i]);
    }

    elem_free(divider1);
    elem_free(divider2);

    printf("part 2: %d\n", part2);

    fclose(fp);
    exit(0);
}

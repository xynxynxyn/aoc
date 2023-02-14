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

int32_t elem_cmp(struct Elem* left, struct Elem* right)
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
    char* left = malloc(buffer_size);
    char* right = malloc(buffer_size);
    int32_t part1_sum = 0;
    int32_t pair_index = 1;
    while (fgets(left, buffer_size, fp) && fgets(right, buffer_size, fp)) {
        // replace newline with null terminator
        left[strcspn(left, "\n")] = 0;
        right[strcspn(right, "\n")] = 0;
        struct Elem* left_elem = elem_parse(left);
        struct Elem* right_elem = elem_parse(right);

        int32_t cmp = elem_cmp(left_elem, right_elem);
        char cmp_char;
        if (cmp == 0) {
            cmp_char = '=';
            part1_sum += pair_index;
        } else if (cmp < 0) {
            cmp_char = '<';
            part1_sum += pair_index;
        } else {
            cmp_char = '>';
        }
        printf("index %c %d\n", cmp_char, pair_index);
        print_elem(left_elem);
        printf("\n");
        print_elem(right_elem);
        printf("\n\n");

        elem_free(left_elem);
        elem_free(right_elem);

        // skip next line
        char empty[buffer_size];
        fgets(empty, buffer_size, fp);
        pair_index++;
    }

    free(left);
    free(right);

    printf("part 1: %d\n", part1_sum);

    fclose(fp);
    exit(0);
}

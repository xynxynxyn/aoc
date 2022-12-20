#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 32
#define DEBUG_BUFFER_SIZE 256

struct Entry {
    int64_t value;
    size_t id;
};

struct RingBuffer {
    struct Entry* array;
    size_t length;
    size_t size;
};

void ring_buffer_init(struct RingBuffer* buffer, size_t n)
{
    buffer->array = calloc(n, sizeof(*buffer->array));
    buffer->length = 0;
    buffer->size = n;
}

void ring_buffer_free(struct RingBuffer* buffer)
{
    free(buffer->array);
}

void ring_buffer_insert(struct RingBuffer* buffer, struct Entry entry)
{
    if (buffer->length >= buffer->size) {
        fprintf(stderr, "ringbuffer ran out of space\n");
        exit(1);
    }
    buffer->array[buffer->length] = entry;
    buffer->length++;
}

void ring_buffer_snprint(char* str, size_t size, struct RingBuffer* buffer)
{
    size_t left = size;
    if (left < 2) {
        return;
    }
    char tmp[32];
    left -= snprintf(str, 2, "[");
    if (buffer->length != 0) {
        snprintf(tmp, 31, "%ld", buffer->array[0].value);
        if (left <= 32) {
            return;
        }
        left = size - strlen(strncat(str, tmp, 32));
    }
    for (size_t i = 1; i < buffer->length; i++) {
        snprintf(tmp, 31, ", %ld", buffer->array[i].value);
        if (left <= 32) {
            return;
        }
        left = size - strlen(strncat(str, tmp, 32));
    }
    if (left < 2) {
        return;
    }
    strncat(str, "]", 2);
}

void ring_buffer_swap(struct RingBuffer* buffer, size_t i, size_t j)
{
    if (i >= buffer->length || j >= buffer->length) {
        fprintf(stderr, "%zu or %zu out of bounds\n", i, j);
        exit(1);
    }
    struct Entry tmp = buffer->array[i];
    buffer->array[i] = buffer->array[j];
    buffer->array[j] = tmp;
}

void ring_buffer_move_to(struct RingBuffer* buffer, int64_t from, int64_t to)
{
    if (from < 0 || from >= buffer->length) {
        fprintf(stderr, "%ld out of bounds\n", from);
        exit(1);
    }
    if (to < 0 || to >= buffer->length) {
        fprintf(stderr, "%ld out of bounds\n", to);
        exit(1);
    }
    if (from == to) {
        return;
    }

    if (to == 0) {
        to = buffer->length - 1;
    }
    if (from < to) {
        // move left to right
        for (size_t i = from; i < to; i++) {
            ring_buffer_swap(buffer, i, i + 1);
        }
    } else {
        // move right to left
        for (size_t i = from; i > to; i--) {
            ring_buffer_swap(buffer, i, i - 1);
        }
    }
}

struct Entry ring_buffer_get(struct RingBuffer* buffer, size_t i)
{
    if (i >= buffer->length) {
        fprintf(stderr, "%zu out of bounds\n", i);
        exit(1);
    }

    return buffer->array[i];
}

bool ring_buffer_contains(struct RingBuffer* buffer, int64_t value)
{
    for (size_t i = 0; i < buffer->length; i++) {
        if (buffer->array[i].value == value) {
            return true;
        }
    }

    return false;
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

void decrypt(struct RingBuffer* r_buffer)
{
    for (size_t id = 0; id < r_buffer->length; id++) {
        // find the first element in the buffer which has not been moved yet
        for (int64_t i = 0; i < r_buffer->length; i++) {
            if (r_buffer->array[i].id != id) {
                // check if this is the value we want to move now
                continue;
            }
            int64_t value = r_buffer->array[i].value;

            int64_t len = r_buffer->length;
            int64_t new_pos = i + value;

            new_pos = (new_pos % (len - 1) + (len - 1)) % (len - 1);

#ifdef DEBUGDECRYPTION
            printf("decrypting %5d at %3zu to %3d", value, i, new_pos);
#endif

            ring_buffer_move_to(r_buffer, i, new_pos);

#ifdef DEBUGDECRYPTION
            char str[DEBUG_BUFFER_SIZE] = { 0 };
            ring_buffer_snprint(str, DEBUG_BUFFER_SIZE, r_buffer);
            printf(" -> %s\n", str);
#endif
            break;
        }
    }
}

int64_t coordinates_sum(struct RingBuffer* r_buffer)
{

    int64_t zero_index = -1;
    for (size_t i = 0; i < r_buffer->length; i++) {
        if (r_buffer->array[i].value == 0) {
            zero_index = i;
            break;
        }
    }
    if (zero_index == -1) {
        fprintf(stderr, "no 0 value in the file\n");
        exit(1);
    }

    int64_t number_1000 = r_buffer->array[(zero_index + 1000) % r_buffer->length].value;
    int64_t number_2000 = r_buffer->array[(zero_index + 2000) % r_buffer->length].value;
    int64_t number_3000 = r_buffer->array[(zero_index + 3000) % r_buffer->length].value;
    return number_1000 + number_2000 + number_3000;
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

    size_t r_buffer_size = number_of_lines(fp);
    struct RingBuffer r_buffer_p1;
    ring_buffer_init(&r_buffer_p1, r_buffer_size);
    struct RingBuffer r_buffer_p2;
    ring_buffer_init(&r_buffer_p2, r_buffer_size);
    int64_t decryption_key = 811589153;
    char buffer[BUFFER_SIZE];
    size_t id = 0;
    while (fgets(buffer, BUFFER_SIZE, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
        struct Entry entry = { 0 };
        entry.id = id;
        if (sscanf(buffer, "%ld", &entry.value) != 1) {
            fprintf(stderr, "could not parse input '%s'\n", buffer);
            exit(1);
        }
        ring_buffer_insert(&r_buffer_p1, entry);
        entry.value *= decryption_key;
        ring_buffer_insert(&r_buffer_p2, entry);
        id++;
    }

#ifdef DEBUG
    char str[DEBUG_BUFFER_SIZE] = { 0 };
    ring_buffer_snprint(str, DEBUG_BUFFER_SIZE, &r_buffer_p1);
    printf("encrypted: %s\n", str);
#endif

    decrypt(&r_buffer_p1);
#ifdef DEBUG
    ring_buffer_snprint(str, DEBUG_BUFFER_SIZE, &r_buffer_p1);
    printf("decrypted: %s\n", str);
#endif

    printf("part 1: %ld\n", coordinates_sum(&r_buffer_p1));

#ifdef DEBUG
    ring_buffer_snprint(str, DEBUG_BUFFER_SIZE, &r_buffer_p2);
    printf("encrypted: %s\n", str);
#endif

    for (size_t i = 0; i < 10; i++) {
        decrypt(&r_buffer_p2);
    }
#ifdef DEBUG
    ring_buffer_snprint(str, DEBUG_BUFFER_SIZE, &r_buffer_p2);
    printf("decrypted: %s\n", str);
#endif

    printf("part 2: %ld\n", coordinates_sum(&r_buffer_p2));

    ring_buffer_free(&r_buffer_p1);
    ring_buffer_free(&r_buffer_p2);
    fclose(fp);
    return 0;
}

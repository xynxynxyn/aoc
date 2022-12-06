#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct CircularBuffer {
    char buffer[32];
    size_t index;
    size_t size;
};

void circ_buf_init(struct CircularBuffer* c_buff, char* chars, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        c_buff->buffer[i] = chars[i];
    }

    c_buff->index = 0;
    c_buff->size = n;
}

bool is_unique(struct CircularBuffer* c_buffer)
{
    for (size_t i = 0; i < c_buffer->size; i++) {
        for (size_t j = i + 1; j < c_buffer->size; j++) {
            if (c_buffer->buffer[i] == c_buffer->buffer[j]) {
                return false;
            }
        }
    }
    return true;
}

void insert(struct CircularBuffer* c_buffer, char c)
{
    c_buffer->buffer[c_buffer->index] = c;
    c_buffer->index++;
    c_buffer->index %= c_buffer->size;
}

size_t unique_sequence_finder(char* buffer, size_t buffer_len, size_t seq_len)
{
    struct CircularBuffer c_buf;
    circ_buf_init(&c_buf, buffer, seq_len);

    if (is_unique(&c_buf)) {
        return seq_len + 1;
    }

    for (size_t i = 4; i < buffer_len; i++) {
        insert(&c_buf, buffer[i]);
        if (is_unique(&c_buf)) {
            return i + 1;
        }
    }

    return 0;
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

    char buffer[6000];
    fgets(buffer, 6000, fp);

    size_t seq_index = unique_sequence_finder(buffer, strlen(buffer), 4);
    printf("%zu\n", seq_index);
    size_t msg_index = unique_sequence_finder(buffer, strlen(buffer), 14);
    printf("%zu\n", msg_index);

    fclose(fp);
    return 0;
}

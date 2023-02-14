#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define VALUES_SIZE 1024

int64_t snafu_to_decimal(char* snafu)
{
    int32_t factor = strlen(snafu);
    char* c = snafu;
    int64_t value = 0;
    while (*c) {
        int64_t v;
        switch (*c) {
        case '2':
            v = 2;
            break;
        case '1':
            v = 1;
            break;
        case '0':
            v = 0;
            break;
        case '-':
            v = -1;
            break;
        case '=':
            v = -2;
            break;
        default:
            fprintf(stderr, "unknown digit '%c'", *c);
            exit(1);
        }
        value += v * (int64_t)pow(5, factor - 1);
        factor--;
        c++;
    }
    return value;
}

void strrev(char* str)
{
    size_t len = strlen(str);
    size_t s = 0;
    size_t e = len - 1;

    for (size_t i = 0; i < len / 2; i++) {
        char tmp = str[s];
        str[s] = str[e];
        str[e] = tmp;
        s++;
        e--;
    }
}

// writes up to tgt_len - 1 bytes to tgt
void decimal_to_snafu(int64_t value, char* tgt, size_t tgt_len)
{
    if (tgt_len < 2) {
        return;
    }
    tgt[0] = 0;

    int64_t exp = 0;
    while (value > 0) {
        int64_t power = (int64_t)pow(5, exp);
        int64_t next_power = (int64_t)pow(5, exp + 1);
        int64_t remainder = value % next_power;
        if (remainder == 2 * power || remainder == 1 * power || remainder == 0) {
            char fmt[2];
            sprintf(fmt, "%ld", remainder / power);
            if (tgt_len < 2) {
                return;
            }
            strncat(tgt, fmt, 2);
            tgt_len--;
            value -= remainder;
        } else if (remainder == 3 * power) {
            // the remainder is 3
            // make this one -2 and let the next section handle it
            if (tgt_len < 2) {
                return;
            }
            strncat(tgt, "=", 2);
            tgt_len--;
            value += 2 * power;
        } else if (remainder == 4 * power) {
            // the remainder is 4
            // same as with 3, but it's only - this time
            if (tgt_len < 2) {
                return;
            }
            strncat(tgt, "-", 2);
            tgt_len--;
            value += 1 * power;
        } else {
            fprintf(stderr, "unreachable\n");
            exit(1);
        }

        exp++;
    }

    strrev(tgt);
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

    int64_t values[VALUES_SIZE] = { 0 };
    size_t values_len = 0;
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, fp)) {
        if (values_len == VALUES_SIZE) {
            fprintf(stderr, "need to allocate more space for input\n");
            exit(1);
        }
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;

        values[values_len] = snafu_to_decimal(buffer);
        values_len++;
    }

    int64_t sum = 0;
    for (size_t i = 0; i < values_len; i++) {
        sum += values[i];
    }

    char sum_snafu[21];
    decimal_to_snafu(sum, sum_snafu, 21);
    printf("%s\n", sum_snafu);

    fclose(fp);
    return 0;
}

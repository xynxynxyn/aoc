#include <stdint.h>
#include <stdio.h>

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
    uint32_t score = 0;
    uint32_t expected_score = 0;
    while (fgets(buffer, buffer_size, fp)) {
        // scanf stuff
        char player;
        char opponent;
        if (sscanf(buffer, "%c %c", &opponent, &player) != 2) {
            // sscanf failed
            fprintf(stderr, "error parsing '%s'\n", buffer);
            return 1;
        } else {
            switch (player) {
            // Rock
            // Lose
            case 'X':
                score += 1;
                switch (opponent) {
                // Rock
                case 'A':
                    score += 3;
                    expected_score += 3;
                    break;
                // Paper
                case 'B':
                    expected_score += 1;
                    break;
                // Scissors
                case 'C':
                    score += 6;
                    expected_score += 2;
                    break;
                }
                break;
            // Paper
            // Draw
            case 'Y':
                score += 2;
                switch (opponent) {
                // Rock
                case 'A':
                    score += 6;
                    expected_score += 4;
                    break;
                // Paper
                case 'B':
                    score += 3;
                    expected_score += 5;
                    break;
                // Scissors
                case 'C':
                    expected_score += 6;
                    break;
                }
                break;
            // Scissors
            // Win
            case 'Z':
                score += 3;
                switch (opponent) {
                // Rock
                case 'A':
                    expected_score += 8;
                    break;
                // Paper
                case 'B':
                    score += 6;
                    expected_score += 9;
                    break;
                // Scissors
                case 'C':
                    score += 3;
                    expected_score += 7;
                    break;
                }
                break;
            default:
                fprintf(stderr, "Unexpected symbol %c", player);
                return 1;
            }
        }
    }

    printf("score: %d\n", score);
    printf("expected score: %d\n", expected_score);

    return 0;
}

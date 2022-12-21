#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 32

enum Operand {
    OP_MUL = '*',
    OP_ADD = '+',
    OP_DIV = '/',
    OP_SUB = '-',
};

enum MonkeyType { MONKEY_NUMBER,
    MONKEY_EQUATION,
    MONKEY_ROOT,
    MONKEY_HUMAN };

struct Monkey {
    char name[5];
    enum MonkeyType type;
    double value;
    enum Operand op;
    struct Monkey* lhs;
    struct Monkey* rhs;
};

void print_equation(struct Monkey* monkey)
{
    switch (monkey->type) {
    case MONKEY_HUMAN:
        printf("h");
        break;
    case MONKEY_NUMBER:
        printf("%.2lf", monkey->value);
        break;
    case MONKEY_ROOT:
        print_equation(monkey->lhs);
        printf(" = ");
        print_equation(monkey->rhs);
        break;
    case MONKEY_EQUATION:
        printf("(");
        print_equation(monkey->lhs);
        printf(" %c ", monkey->op);
        print_equation(monkey->rhs);
        printf(")");
        break;
    default:
        fprintf(stderr, "unknown monkey type for monkey '%s'\n", monkey->name);
        exit(1);
    }
}

// returns true if the formula was modified
bool monkey_simplify(struct Monkey* monkey)
{
    if (monkey->type == MONKEY_NUMBER || monkey->type == MONKEY_HUMAN) {
        return false;
    }

    if (monkey->type == MONKEY_ROOT) {
        struct Monkey* number = NULL;
        struct Monkey* formula = NULL;
        if (monkey->lhs->type == MONKEY_NUMBER) {
            number = monkey->lhs;
            formula = monkey->rhs;
        } else if (monkey->rhs->type == MONKEY_NUMBER) {
            number = monkey->rhs;
            formula = monkey->lhs;
        }

        if (number != NULL && formula != NULL && formula->type != MONKEY_HUMAN) {
            struct Monkey* formula_number = NULL;
            struct Monkey* formula_formula = NULL;
            if (formula->lhs->type == MONKEY_NUMBER) {
                formula_number = formula->lhs;
                formula_formula = formula->rhs;
            }
            if (formula->rhs->type == MONKEY_NUMBER) {
                formula_number = formula->rhs;
                formula_formula = formula->lhs;
            }

            if (formula_number != NULL && formula_formula != NULL) {
                // we can balance the sides out now
                monkey->lhs = formula_formula;
                monkey->rhs = number;
                switch (formula->op) {
                case OP_ADD:
                    monkey->rhs->value = number->value - formula_number->value;
                    break;
                case OP_SUB:
                    if (formula_number == formula->rhs) {
                        // f - x = y -> f = y + x
                        monkey->rhs->value = number->value + formula_number->value;
                    } else {
                        // x - f = y -> f = -y + x
                        monkey->rhs->value = -number->value + formula_number->value;
                    }
                    break;
                case OP_MUL:
                    monkey->rhs->value = number->value / formula_number->value;
                    break;
                case OP_DIV:
                    monkey->rhs->value = number->value * formula_number->value;
                    break;
                default:
                    fprintf(stderr, "unknown operand type for '%s'\n", formula->name);
                    exit(1);
                }

                return true;
            }
        }
    }

    if (monkey_simplify(monkey->rhs) || monkey_simplify(monkey->lhs)) {
        return true;
    };

    bool modified = false;
    if (monkey->type == MONKEY_EQUATION
            && monkey->lhs->type == MONKEY_NUMBER
            && monkey->rhs->type == MONKEY_NUMBER) {
        // compute
        monkey->type = MONKEY_NUMBER;
        switch (monkey->op) {
        case OP_ADD:
            monkey->value = monkey->lhs->value + monkey->rhs->value;
            break;
        case OP_DIV:
            monkey->value = monkey->lhs->value / monkey->rhs->value;
            break;
        case OP_MUL:
            monkey->value = monkey->lhs->value * monkey->rhs->value;
            break;
        case OP_SUB:
            monkey->value = monkey->lhs->value - monkey->rhs->value;
            break;
        default:
            fprintf(stderr, "unknown operand for monkey %s\n", monkey->name);
            exit(1);
        }
        modified = true;
    } else if (monkey->type == MONKEY_EQUATION) {
        // do other simplifications
        struct Monkey* formula;
        struct Monkey* number;
        struct Monkey* inner_number;
        struct Monkey* inner_formula;
        if (monkey->lhs->type == MONKEY_NUMBER) {
            formula = monkey->rhs;
            number = monkey->lhs;
        } else if (monkey->rhs->type == MONKEY_NUMBER) {
            number = monkey->rhs;
            formula = monkey->lhs;
        } else {
            fprintf(stderr, "two unknowns for monkey '%s'\n", monkey->name);
            exit(1);
        }
        if (formula->type != MONKEY_EQUATION) {
            return false;
        }
        if (formula->lhs->type == MONKEY_NUMBER) {
            inner_number = formula->lhs;
            inner_formula = formula->rhs;
        } else if (formula->rhs->type == MONKEY_NUMBER) {
            inner_number = formula->rhs;
            inner_formula = formula->lhs;
        } else {
            fprintf(stderr, "no number present inside formula\n");
            exit(1);
        }

        // if the right hand side of a substraction is a number,
        // make it an addition instead
        if (formula->op == OP_SUB && inner_number == formula->rhs) {
#ifdef DEBUG
            printf("-> (f - x) -> (f + -x)\n");
#endif
            modified = true;
            formula->op = OP_ADD;
            inner_number->value *= -1;
        } else if (formula->op == OP_DIV
                && inner_number == formula->rhs) {
#ifdef DEBUG
            printf("-> (f / x) -> (f * z)\n");
#endif
            modified = true;
            formula->op = OP_MUL;
            inner_number->value = 1 / inner_number->value;
        } else if (monkey->op == OP_ADD && (formula->op == OP_ADD || formula->op == OP_SUB)) {
#ifdef DEBUG
            printf("-> ((f + x) + y) -> ((f + z))\n");
#endif
            modified = true;
            // associative law for addition
            // due to the previous if statement numbers are always on the lhs in substractions
            // making this operation correct
            monkey->op = formula->op;
            int32_t increment = number->value;
            monkey->lhs = formula->lhs;
            monkey->rhs = formula->rhs;

            if (monkey->lhs->type == MONKEY_NUMBER) {
                monkey->lhs->value += increment;
            } else if (monkey->rhs->type == MONKEY_NUMBER) {
                monkey->rhs->value += increment;
            }
        } else if (monkey->op == OP_MUL && (formula->op == OP_ADD || formula->op == OP_SUB)) {
#ifdef DEBUG
            printf("-> ((f + x) * y) -> ((f * y) + z)\n");
#endif
            modified = true;
            // distributivity law
            // one of the two sides must be a number since the unknown only exists once

            // ensure the formula is on the left
            monkey->lhs = formula;
            monkey->rhs = number;

            double tmp = number->value;
            if (monkey->op == OP_MUL) {
                number->value = inner_number->value * number->value;
            } else {
                number->value = inner_number->value / number->value;
            }
            inner_number->value = tmp;

            // transform the formula into the new multiplication
            // by swapping the operands
            enum Operand tmp_op = formula->op;
            formula->op = monkey->op;
            monkey->op = tmp_op;
        } else if (monkey->op == OP_DIV && number == monkey->rhs) {
            // we divide a formula
            if (formula->op == OP_MUL) {
#ifdef DEBUG
                printf("-> ((f * x) / y) -> (f / z)\n");
#endif
                modified = true;
                monkey->rhs->value = inner_number->value / number->value;
                monkey->lhs = inner_formula;
            } else if (formula->op == OP_DIV && inner_number == formula->rhs) {
#ifdef DEBUG
                printf("-> ((f / x) / y) -> (f / z)\n");
#endif
                modified = true;
                monkey->rhs->value = number->value / inner_number->value;
                monkey->lhs = inner_formula;
            }
        } else if (monkey->op == OP_MUL && formula->op == OP_DIV && inner_number == formula->rhs) {
#ifdef DEBUG
            printf("-> ((f / x) * y) -> (f * (y / x))\n");
#endif
            modified = true;
            monkey->rhs = number;
            monkey->rhs->value = number->value / inner_number->value;
            monkey->op = OP_MUL;
            monkey->lhs = inner_formula;
        } else if (monkey->op == OP_MUL && formula->op == OP_DIV && inner_number == formula->lhs) {
#ifdef DEBUG
            printf("-> ((x / f) * y) -> ((x * y) / f)\n");
#endif
            modified = true;
            monkey->lhs = number;
            monkey->lhs->value = number->value * inner_number->value;
            monkey->rhs = inner_formula;
        } else if (monkey->op == OP_MUL && formula->op == OP_MUL) {
#ifdef DEBUG
            printf("-> ((f * x) * y) -> (f * (x * y))\n");
#endif
            modified = true;
            monkey->lhs = inner_formula;
            monkey->rhs = number;
            monkey->rhs->value = inner_number->value * number->value;
        }
    }
    return modified;
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
        int32_t value;
        struct Monkey* monkey = &monkeys[monkey_index];
        if (sscanf(buffer[monkey_index], "%4s: %d", monkey->name, &value) == 2) {
            if (strncmp(monkey->name, "humn", 4) == 0) {
                monkey->type = MONKEY_HUMAN;
            } else {
                monkey->name[4] = 0;
                // got a number monkey
                monkey->type = MONKEY_NUMBER;
            }
            monkey->value = (double)value;
            monkey->lhs = NULL;
            monkey->rhs = NULL;
            monkey->op = 0;
        } else if (sscanf(buffer[monkey_index], "%4s: %4s %c %4s", monkey->name, lhs, &operand, rhs) == 4) {
            monkey->value = 0;
            // find the other monkeys in the computation already if we can
            monkey->lhs = find_monkey_with_name(monkeys, monkey_index, lhs);
            monkey->rhs = find_monkey_with_name(monkeys, monkey_index, rhs);

            if (strncmp(monkey->name, "root", 4) == 0) {
                monkey->type = MONKEY_ROOT;
            } else {
                monkey->type = MONKEY_EQUATION;
                // got a computation monkey
                switch (operand) {
                case '*':
                    monkey->op = OP_MUL;
                    break;
                case '+':
                    monkey->op = OP_ADD;
                    break;
                case '-':
                    monkey->op = OP_SUB;
                    break;
                case '/':
                    monkey->op = OP_DIV;
                    break;
                default:
                    fprintf(stderr, "unknown monkey type '%s'\n", buffer[monkey_index]);
                    break;
                }
            }
        }
        monkey_index++;
    }

    // resolve the remaining monkey dependencies
    for (size_t i = 0; i < monkeys_len; i++) {
        struct Monkey* monkey = &monkeys[i];
        if (monkey->type == MONKEY_EQUATION || monkey->type == MONKEY_ROOT) {
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
        if (monkeys[i].type == MONKEY_ROOT) {
            // found root monkey
            root_monkey = &monkeys[i];
            break;
        }
    }

    if (root_monkey == NULL) {
        fprintf(stderr, "could not find root monkey\n");
        exit(1);
    }

    print_equation(root_monkey);
    printf("\n");
    while (monkey_simplify(root_monkey)) {
        print_equation(root_monkey);
        printf("\n");
    }

    fclose(fp);
    return 0;
}

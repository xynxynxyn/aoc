#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_LEN 1024
#define QUEUE_SIZE 1024
#define DYN_PROG_SIZE 256
#define TIME 26

struct Valve {
    char name[3];
    int32_t flow_rate;
    struct Valve** connections;
    size_t connections_len;
    size_t connections_size;
    size_t id;
};

struct Actor {
    struct Valve* cur;
    struct Valve* goal;
    int32_t steps_remaining;
};

bool add_connection_by_id(struct Valve* valve, struct Valve* valves, size_t valves_len, char* id)
{
    if (valve->connections_len >= valve->connections_size) {
        fprintf(stderr, "buffer overflow\n");
        exit(1);
    }
    for (size_t v = 0; v < valves_len; v++) {
        if (strncmp(valves[v].name, id, 2) == 0) {
            // found it
            valve->connections[valve->connections_len] = &valves[v];
            valve->connections_len++;
            return true;
        }
    }

    return false;
}

struct Queue {
    void* array[QUEUE_SIZE];
    size_t first;
    size_t length;
};

void* queue_pop(struct Queue* queue)
{
    if (queue->length == 0) {
        fprintf(stderr, "trying to pop from empty queue\n");
        exit(1);
    }

    struct Valve* v = queue->array[queue->first];
    queue->first = (queue->first + 1) % QUEUE_SIZE;
    queue->length--;
    return v;
}

void queue_push(struct Queue* queue, void* valve)
{
    if (queue->length == QUEUE_SIZE) {
        fprintf(stderr, "queue ran out of space\n");
        exit(1);
    }
    queue->array[(queue->first + queue->length) % QUEUE_SIZE] = valve;
    queue->length++;
}

bool queue_contains(struct Queue* queue, void* valve)
{
    for (int32_t i = 0; i < queue->length; i++) {
        if (queue->array[(queue->first + i) % QUEUE_SIZE] == valve) {
            return true;
        }
    }

    return false;
}

int32_t calculate_distance(struct Valve* start, struct Valve* target)
{
    static int32_t distances[DYN_PROG_SIZE][DYN_PROG_SIZE] = { 0 };

    if (distances[start->id][target->id] != 0) {
        return distances[start->id][target->id];
    }

    // do breadth first search to measure distance
    struct Queue queue = { 0 };
    struct Queue depth_queue = { 0 };
    queue_push(&queue, start);
    queue_push(&depth_queue, 0);
    int32_t depth = 0;

    struct Valve* current;
    while (queue.length != 0) {
        // pop
        current = queue_pop(&queue);
        depth = (intptr_t)queue_pop(&depth_queue) % INT32_MAX;

        // search
        if (current == target) {
            break;
        }

        for (size_t i = 0; i < current->connections_len; i++) {
            if (!queue_contains(&queue, current->connections[i])) {
                queue_push(&queue, current->connections[i]);
                queue_push(&depth_queue, (void*)(intptr_t)depth + 1);
            }
        }
    }

    distances[start->id][current->id] = depth;
    return (int32_t)depth;
}

int32_t _calc_max_flow(struct Actor you, struct Actor elephant, struct Valve* valves,
    size_t valves_len, bool* valve_status, int32_t remaining_time, char* path)
{
    char rec_path[STR_LEN] = { 0 };
    bool new_valve_status[valves_len];
    memcpy(new_valve_status, valve_status, sizeof(bool) * valves_len);

    // do the steps if actors are busy
    // do multiple steps at a time
    // no need to go one by one
    if (remaining_time <= 1) {
        return 0;
    }
    // check that at least one valve is still open
    bool one_closed = false;
    for (size_t v = 0; v < valves_len; v++) {
        if (valves[v].flow_rate > 0 && !new_valve_status[v]) {
            one_closed = true;
            break;
        }
    }
    if (!one_closed) {
        return 0;
    }

    int32_t steps = you.steps_remaining < elephant.steps_remaining ? you.steps_remaining : elephant.steps_remaining;

    // if the amount of time left to reach a goal is too long also stop
    if (remaining_time - steps <= 1) {
        return 0;
    }

    bool opening_occured = false;
    int32_t generated_flow = 0;
    char fmt_y[128] = { 0 };
    char fmt_e[128] = { 0 };

    // update step count
    remaining_time -= steps;
    you.steps_remaining -= steps;
    elephant.steps_remaining -= steps;

    // due to opening a valve
    remaining_time--;

    if (you.steps_remaining == 0 && you.goal != NULL) {
        new_valve_status[you.goal->id] = true;
        int32_t flow = you.goal->flow_rate * remaining_time;
        generated_flow += flow;
        sprintf(fmt_y, " -> y%s@%d+%d(%df*%dm)",
            you.goal->name, TIME - remaining_time, flow, you.goal->flow_rate, remaining_time);
        you.cur = you.goal;
        you.goal = NULL;
    } else if (you.steps_remaining > 0 && you.goal != NULL) {
        you.steps_remaining--;
    }

    if (elephant.steps_remaining == 0 && elephant.goal != NULL) {
        new_valve_status[elephant.goal->id] = true;
        int32_t flow = elephant.goal->flow_rate * remaining_time;
        generated_flow += flow;
        sprintf(fmt_e, " -> e%s@%d+%d(%df*%dm)",
            elephant.goal->name, TIME - remaining_time, flow, elephant.goal->flow_rate, remaining_time);
        elephant.cur = elephant.goal;
        elephant.goal = NULL;
    } else if (elephant.steps_remaining > 0 && elephant.goal != NULL) {
        elephant.steps_remaining--;
    }

    // determining next steps
    int32_t max_flow = 0;
    size_t remaining_valves = 0;
    for (size_t v = 0; v < valves_len; v++) {
        if (valves[v].flow_rate > 0 && !new_valve_status[v]
            && elephant.goal != &valves[v] && you.goal != &valves[v]) {
            remaining_valves++;
        }
    }
    if (remaining_valves == 0 && (elephant.goal != NULL || you.goal != NULL)) {
        // no remaining valves for anyone to go to
        // just keep simulating without assigning him
        char tmp_path[STR_LEN] = { 0 };
        int32_t flow = _calc_max_flow(you, elephant, valves, valves_len, new_valve_status, remaining_time, tmp_path);
        if (flow > max_flow) {
            max_flow = flow;
            strncpy(rec_path, tmp_path, STR_LEN - 1);
        }
    }
    if (you.goal == NULL && elephant.goal != NULL) {
        // find goal for you
        for (size_t v = 0; v < valves_len; v++) {
            if (new_valve_status[v] || valves[v].flow_rate == 0 || elephant.goal == &valves[v]) {
                // if the valve has no flow, is already open or the elephant is targetting at, don't
                continue;
            }

            you.goal = &valves[v];
            you.steps_remaining = calculate_distance(you.cur, you.goal);
            char tmp_path[STR_LEN] = { 0 };
            int32_t flow = _calc_max_flow(you, elephant, valves, valves_len, new_valve_status, remaining_time, tmp_path);
            if (flow > max_flow) {
                max_flow = flow;
                strncpy(rec_path, tmp_path, STR_LEN - 1);
            }
        }
    } else if (you.goal != NULL && elephant.goal == NULL) {
        // find goal for the elephant
        for (size_t v = 0; v < valves_len; v++) {
            if (new_valve_status[v] || valves[v].flow_rate == 0 || you.goal == &valves[v]) {
                // if the valve has no flow, is already open or the elephant is targetting at, don't
                continue;
            }

            elephant.goal = &valves[v];
            elephant.steps_remaining = calculate_distance(elephant.cur, elephant.goal);
            char tmp_path[STR_LEN] = { 0 };
            int32_t flow = _calc_max_flow(you, elephant, valves, valves_len, new_valve_status, remaining_time, tmp_path);
            if (flow > max_flow) {
                max_flow = flow;
                strncpy(rec_path, tmp_path, STR_LEN - 1);
            }
        }
    } else if (you.goal == NULL && elephant.goal == NULL) {
        if (remaining_valves == 1) {
            // only one closed valve remaining
            for (size_t v = 0; v < valves_len; v++) {
                if (valves[v].flow_rate > 0 && !new_valve_status[v]) {
                    // test going to it with both the player and the elephant
                    you.goal = &valves[v];
                    you.steps_remaining = calculate_distance(you.cur, you.goal);
                    char tmp_path_y[STR_LEN] = { 0 };
                    int32_t flow = _calc_max_flow(you, elephant, valves, valves_len, new_valve_status, remaining_time, tmp_path_y);
                    if (flow > max_flow) {
                        max_flow = flow;
                        strncpy(rec_path, tmp_path_y, STR_LEN - 1);
                    }

                    // test with elephant after resetting player first
                    you.goal = NULL;
                    you.steps_remaining = 0;
                    elephant.goal = &valves[v];
                    elephant.steps_remaining = calculate_distance(elephant.cur, elephant.goal);
                    char tmp_path_e[STR_LEN] = { 0 };
                    flow = _calc_max_flow(you, elephant, valves, valves_len, new_valve_status, remaining_time, tmp_path_e);
                    if (flow > max_flow) {
                        max_flow = flow;
                        strncpy(rec_path, tmp_path_e, STR_LEN - 1);
                    }
                }
            }
        } else {
            // find goal for both you and the elephant
            for (size_t v0 = 0; v0 < valves_len; v0++) {
                for (size_t v1 = v0 + 1; v1 < valves_len; v1++) {
                    if (valves[v0].flow_rate == 0 || new_valve_status[v0] || &valves[v0] == you.cur || &valves[v0] == elephant.cur) {
                        break;
                    }
                    if (valves[v1].flow_rate == 0 || new_valve_status[v1] || &valves[v1] == you.cur || &valves[v1] == elephant.cur || v0 == v1) {
                        continue;
                    }

                    // found a pair of targets
                    you.goal = &valves[v0];
                    you.steps_remaining = calculate_distance(you.cur, you.goal);
                    elephant.goal = &valves[v1];
                    elephant.steps_remaining = calculate_distance(elephant.cur, elephant.goal);
                    char tmp_path_0[STR_LEN] = { 0 };
                    int32_t flow = _calc_max_flow(you, elephant, valves, valves_len, new_valve_status, remaining_time, tmp_path_0);

                    if (flow > max_flow) {
                        max_flow = flow;
                        strncpy(rec_path, tmp_path_0, STR_LEN - 1);
                    }

                    you.goal = &valves[v1];
                    you.steps_remaining = calculate_distance(you.cur, you.goal);
                    elephant.goal = &valves[v0];
                    elephant.steps_remaining = calculate_distance(elephant.cur, elephant.goal);
                    char tmp_path_1[STR_LEN] = { 0 };
                    flow = _calc_max_flow(you, elephant, valves, valves_len, new_valve_status, remaining_time, tmp_path_1);

                    if (flow > max_flow) {
                        max_flow = flow;
                        strncpy(rec_path, tmp_path_1, STR_LEN - 1);
                    }
                }
            }
        }
    } else {
        fprintf(stderr, "unreachable\n");
        exit(1);
    }

    strcat(path, fmt_y);
    strcat(path, fmt_e);
    strcat(path, rec_path);
    return generated_flow + max_flow;
}

int32_t calc_max_flow(struct Actor you, struct Actor elephant, struct Valve* valves,
    size_t valves_len, bool* valve_status, int32_t remaining_time, char* path)
{

    char rec_path[STR_LEN] = { 0 };
    int32_t max_flow = 0;
    // find first targets
    for (size_t v0 = 0; v0 < valves_len; v0++) {
        for (size_t v1 = v0 + 1; v1 < valves_len; v1++) {
            if (valves[v0].flow_rate == 0) {
                break;
            }
            if (valves[v1].flow_rate == 0) {
                continue;
            }

            // found a pair of targets
            you.goal = &valves[v0];
            you.steps_remaining = calculate_distance(you.cur, you.goal);
            elephant.goal = &valves[v1];
            elephant.steps_remaining = calculate_distance(elephant.cur, elephant.goal);
            char tmp_path_0[STR_LEN] = { 0 };
            int32_t flow = _calc_max_flow(you, elephant, valves, valves_len, valve_status, remaining_time, tmp_path_0);

            if (flow > max_flow) {
                max_flow = flow;
                strncpy(rec_path, tmp_path_0, STR_LEN - 1);
            }
        }
    }

    strcat(path, you.cur->name);
    strcat(path, rec_path);
    return max_flow;
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

    struct Valve valves[256] = { 0 };
    size_t valves_len = 0;

    char buffer[256];
    char lines[256][256];
    size_t number_of_lines = 0;
    while (fgets(buffer, 256, fp)) {
        if (number_of_lines > 255) {
            fprintf(stderr, "too many input lines\n");
        }
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        strncpy(lines[number_of_lines], buffer, 255);
        lines[number_of_lines][255] = 0;
        number_of_lines++;
    }

    for (size_t i = 0; i < number_of_lines; i++) {
        char* line = lines[i];
        struct Valve* valve = &valves[valves_len];
        // parse first section
        if (sscanf(line, "Valve %2s has flow rate=%d", valve->name, &valve->flow_rate) != 2) {
            fprintf(stderr, "could not parse %s\n", buffer);
            exit(1);
        }
        valve->name[2] = 0;
        valve->connections = calloc(8, sizeof(*valve->connections));
        valve->connections_len = 0;
        valve->connections_size = 8;
        valve->id = i;
        valves_len++;
    }

    for (size_t i = 0; i < number_of_lines; i++) {
        char* line = lines[i];
        struct Valve* valve = &valves[i];
        char* tunnels = line + strcspn(lines[i], ";");
        if (tunnels[8] == 's') {
            // multiple tunnels
            tunnels += 24;
        } else {
            // single tunnel
            tunnels += 23;
        }

        char* t = strtok(tunnels, ",");
        while (t != NULL) {
            // find valve with that id
            add_connection_by_id(valve, valves, valves_len, t + 1);
            t = strtok(NULL, ",");
        }
    }

    /* PARSING DONE */

    struct Valve* start = NULL;

    for (size_t i = 0; i < valves_len; i++) {
        if (strncmp(valves[i].name, "AA", 2) == 0) {
            start = &valves[i];
            break;
        }
    }

    if (start == NULL) {
        fprintf(stderr, "no valve with label AA\n");
        exit(1);
    }

    int32_t investment;
    bool valve_status[valves_len];
    memset(valve_status, 0, sizeof(*valve_status) * valves_len);

    struct Actor you = { .cur = start };
    struct Actor elephant = { .cur = start };

    char path[STR_LEN] = { 0 };
    printf("maximum flow: %d\n", calc_max_flow(you, elephant, valves, valves_len, valve_status, TIME, path));
    printf("%s\n", path);

    // free
    for (size_t i = 0; i < valves_len; i++) {
        free(valves[i].connections);
    }
    fclose(fp);
    return 0;
}

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_LEN 512
#define QUEUE_SIZE 1024
#define DYN_PROG_SIZE 256
#define TIME 30

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
#ifdef DYNAMIC
    static int32_t distances[DYN_PROG_SIZE][DYN_PROG_SIZE] = { 0 };

    if (distances[start->id][target->id] != 0) {
        return distances[start->id][target->id];
    }
#endif

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

#ifdef DYNAMIC
    distances[start->id][current->id] = depth;
#endif
    return (int32_t)depth;
}

int32_t _calc_max_flow(struct Actor you, struct Valve* valves,
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
    // check that at least one valve is open
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
    // if the amount of time left to reach the goal is too long also stop
    if (remaining_time - you.steps_remaining <= 1) {
        return 0;
    }

    // arrived at goal valve, open it
    remaining_time -= you.steps_remaining;
    new_valve_status[you.goal->id] = true;
    int32_t generated_flow = you.goal->flow_rate * (remaining_time - 1);

    int32_t max_flow = 0;
    you.cur = you.goal;
    // find a new target
    for (size_t v = 0; v < valves_len; v++) {
        if (valves[v].flow_rate == 0 || new_valve_status[v] || &valves[v] == you.cur) {
            // skip valves which are already open, have 0 flow or are already targeted by an actor
            continue;
        }

        you.goal = &valves[v];
        int32_t distance = calculate_distance(you.cur, you.goal);
        you.steps_remaining = distance;
        char tmp_path[STR_LEN] = { 0 };
        // - 1 for opening the current valve
        int32_t flow = _calc_max_flow(you, valves, valves_len, new_valve_status, remaining_time - 1, tmp_path);
        if (flow > max_flow) {
            max_flow = flow;
            strcpy(rec_path, tmp_path);
        }
    }

    // log
    char fmt[32];
    sprintf(fmt, " -> %s@%dm +%d", you.cur->name, TIME - remaining_time + 1, generated_flow);
    strcat(path, fmt);
    strcat(path, rec_path);
    return generated_flow + max_flow;
}

int32_t calc_max_flow(struct Actor you, struct Valve* valves,
    size_t valves_len, bool* valve_status, int32_t remaining_time, char* path)
{

    char rec_path[STR_LEN] = { 0 };
    int32_t max_flow = 0;
    // find a new target
    for (size_t v = 0; v < valves_len; v++) {
        if (valves[v].flow_rate == 0 || &valves[v] == you.cur) {
            // skip valves which are already open, have 0 flow or are already targeted by an actor
            continue;
        }

        you.goal = &valves[v];
        int32_t distance = calculate_distance(you.cur, you.goal);
        you.steps_remaining = distance;

        char tmp_path[STR_LEN] = { 0 };
        int32_t flow = _calc_max_flow(you, valves, valves_len, valve_status, remaining_time, tmp_path);
        if (flow > max_flow) {
            max_flow = flow;
            strcpy(rec_path, tmp_path);
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

    char path[STR_LEN];
    printf("maximum flow: %d\n", calc_max_flow(you, valves, valves_len, valve_status, TIME, path));
    printf("%s\n", path);

    // free
    for (size_t i = 0; i < valves_len; i++) {
        free(valves[i].connections);
    }
    fclose(fp);
    return 0;
}

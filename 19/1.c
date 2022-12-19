#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define BLUEPRINTS_SIZE 32
#define TIME 24

struct Resources {
    int32_t ore;
    int32_t clay;
    int32_t obsidian;
    int32_t geodes;
};

struct Blueprint {
    struct Resources ore_robot_cost;
    struct Resources clay_robot_cost;
    struct Resources obsidian_robot_cost;
    struct Resources geode_robot_cost;
};

struct State {
    struct Resources stock;
    struct Blueprint blueprint;
    size_t ore_robots;
    size_t clay_robots;
    size_t obsidian_robots;
    size_t geode_robots;
    int32_t minutes_remaining;
};

int32_t _max_geode_yield(struct State state, bool constructed)
{
    if (state.minutes_remaining <= 1) {
        // update current stock and return final yield
        state.stock.ore += state.ore_robots;
        state.stock.clay += state.clay_robots;
        state.stock.obsidian += state.obsidian_robots;
        state.stock.geodes += state.geode_robots;
        return state.stock.geodes;
    }

    struct Blueprint bp = state.blueprint;

    // pass 1 minute
    state.minutes_remaining--;
    int32_t max_yield = 0;

    // go through all legal moves recursively
    // assume you can only build a single robot per turn

    bool can_build_geode_robot = bp.geode_robot_cost.obsidian <= state.stock.obsidian
        && bp.geode_robot_cost.ore <= state.stock.ore;
    bool could_have_built_geode_before = !constructed
        && (state.stock.ore - state.ore_robots >= bp.geode_robot_cost.ore)
        && (state.stock.obsidian - state.obsidian_robots >= bp.geode_robot_cost.obsidian);
    if (state.minutes_remaining > 0
        && can_build_geode_robot) {
        // we can build a geode robot
        struct State new_state = state;
        new_state.stock.ore -= bp.geode_robot_cost.ore;
        new_state.stock.obsidian -= bp.geode_robot_cost.obsidian;

        // update current stock
        new_state.stock.ore += new_state.ore_robots;
        new_state.stock.clay += new_state.clay_robots;
        new_state.stock.obsidian += new_state.obsidian_robots;
        new_state.stock.geodes += new_state.geode_robots;

        // build the robot
        new_state.geode_robots++;

        int32_t yield = _max_geode_yield(new_state, true);
        if (yield > max_yield) {
            max_yield = yield;
            // building a geode robot is always optimal
            goto end_turn;
        }
    }

    bool can_build_obsidian_robot = bp.obsidian_robot_cost.clay <= state.stock.clay
        && bp.obsidian_robot_cost.ore <= state.stock.ore;
    bool could_have_built_obsidian_before = !constructed
        && (state.stock.ore - state.ore_robots >= bp.obsidian_robot_cost.ore)
        && (state.stock.clay - state.clay_robots >= bp.obsidian_robot_cost.clay);
    if (state.minutes_remaining > 1
        && !could_have_built_obsidian_before
        && can_build_obsidian_robot) {
        // we can build an obsidian robot
        struct State new_state = state;
        new_state.stock.ore -= bp.obsidian_robot_cost.ore;
        new_state.stock.clay -= bp.obsidian_robot_cost.clay;

        // update current stock
        new_state.stock.ore += new_state.ore_robots;
        new_state.stock.clay += new_state.clay_robots;
        new_state.stock.obsidian += new_state.obsidian_robots;
        new_state.stock.geodes += new_state.geode_robots;

        // build the robot
        new_state.obsidian_robots++;

        int32_t yield = _max_geode_yield(new_state, true);
        if (yield > max_yield) {
            max_yield = yield;
        }
    }

    bool can_build_clay_robot = bp.clay_robot_cost.ore <= state.stock.ore;
    bool could_have_built_clay_before = !constructed
        && (state.stock.ore - state.ore_robots >= bp.clay_robot_cost.ore);
    if (state.minutes_remaining > 2
        && !could_have_built_clay_before
        && can_build_clay_robot) {
        // since there is 1 construction step between a clay robot and a geode robot
        // it is of no value to produce one now
        // we can build a clay robot
        struct State new_state = state;
        new_state.stock.ore -= bp.clay_robot_cost.ore;

        // update current stock
        new_state.stock.ore += new_state.ore_robots;
        new_state.stock.clay += new_state.clay_robots;
        new_state.stock.obsidian += new_state.obsidian_robots;
        new_state.stock.geodes += new_state.geode_robots;

        // build the robot
        new_state.clay_robots++;

        int32_t yield = _max_geode_yield(new_state, true);
        if (yield > max_yield) {
            max_yield = yield;
        }
    }

    bool can_build_ore_robot = bp.ore_robot_cost.ore <= state.stock.ore;
    bool could_have_built_ore_before = !constructed
        && (state.stock.ore - state.ore_robots >= bp.ore_robot_cost.ore);
    if (state.minutes_remaining > 1
        && !could_have_built_ore_before
        && can_build_ore_robot) {
        // we can build an ore robot
        struct State new_state = state;
        new_state.stock.ore -= bp.ore_robot_cost.ore;

        // update current stock
        new_state.stock.ore += new_state.ore_robots;
        new_state.stock.clay += new_state.clay_robots;
        new_state.stock.obsidian += new_state.obsidian_robots;
        new_state.stock.geodes += new_state.geode_robots;

        // build the robot
        new_state.ore_robots++;

        int32_t yield = _max_geode_yield(new_state, true);
        if (yield > max_yield) {
            max_yield = yield;
        }
    }

    // determine whether we could have built all possible robots
    // if this is the case it would be inefficient to not build a robot
    // therefore, don't explore the path where don't build anything
    bool have_to_build = (state.clay_robots == 0 && can_build_clay_robot && can_build_ore_robot)
        || (state.obsidian_robots == 0 && can_build_clay_robot && can_build_ore_robot && can_build_obsidian_robot)
        || (can_build_ore_robot && can_build_clay_robot && can_build_obsidian_robot && can_build_geode_robot);
    if (!have_to_build) {
        // update current stock
        state.stock.ore += state.ore_robots;
        state.stock.clay += state.clay_robots;
        state.stock.obsidian += state.obsidian_robots;
        state.stock.geodes += state.geode_robots;

        // test the path where we do nothing
        int32_t yield = _max_geode_yield(state, false);
        if (yield >= max_yield) {
            max_yield = yield;
        }
    }

end_turn:
    return max_yield;
}

int32_t max_geode_yield(struct Blueprint blueprint)
{
    // construct the state
    struct State init_state = {
        .ore_robots = 1,
        .blueprint = blueprint,
        .minutes_remaining = TIME,
    };

    int32_t yield = _max_geode_yield(init_state, false);
    return yield;
}

struct ThreadArgs {
    struct Blueprint blueprint;
    int32_t* yield;
};

void* max_yield_thread(void* pargs)
{
    struct ThreadArgs* args = pargs;
    *(args->yield) = max_geode_yield(args->blueprint);

    return NULL;
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

    char buffer[BUFFER_SIZE] = { 0 };
    struct Blueprint blueprints[BLUEPRINTS_SIZE] = { 0 };
    size_t blueprints_len = 0;
    while (fgets(buffer, BUFFER_SIZE, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;
        // sscanf stuff
        struct Blueprint* bp = &blueprints[blueprints_len];
        char* bp_str = strtok(buffer + strcspn(buffer, ":") + 1, ".");
        int32_t ore, clay, obsidian;
        while (bp_str) {
            // skip the first space
            if (sscanf(bp_str, " Each ore robot costs %d ore", &ore) == 1) {
                bp->ore_robot_cost.ore = ore;
            } else if (sscanf(bp_str, " Each clay robot costs %d ore", &ore) == 1) {
                bp->clay_robot_cost.ore = ore;
            } else if (sscanf(bp_str, " Each obsidian robot costs %d ore and %d clay", &ore, &clay) == 2) {
                bp->obsidian_robot_cost.ore = ore;
                bp->obsidian_robot_cost.clay = clay;
            } else if (sscanf(bp_str, " Each geode robot costs %d ore and %d obsidian", &ore, &obsidian) == 2) {
                bp->geode_robot_cost.ore = ore;
                bp->geode_robot_cost.obsidian = obsidian;
            } else {
                fprintf(stderr, "could not parse blueprint '%s'\n", bp_str);
                exit(1);
            }
            bp_str = strtok(NULL, ".");
        }

        blueprints_len++;
    }

    int32_t quality_sum = 0;
    pthread_t threads[blueprints_len];
    int32_t yields[blueprints_len];
    struct ThreadArgs args[blueprints_len];
    for (size_t i = 0; i < blueprints_len; i++) {
        args[i].blueprint = blueprints[i];
        yields[i] = 0;
        args[i].yield = &yields[i];
        pthread_create(&threads[i], NULL, max_yield_thread, &args[i]);
    }

    for (size_t i = 0; i < blueprints_len; i++) {
        pthread_join(threads[i], NULL);
        // printf("yield for blueprint %zu: %d\n", i + 1, yields[i]);
        quality_sum += yields[i] * (i + 1);
    }

    printf("quality sum: %d\n", quality_sum);

    fclose(fp);
    return 0;
}

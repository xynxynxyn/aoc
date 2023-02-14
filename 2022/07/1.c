#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct File {
    int32_t size;
    char name[16];
};

struct Directory {
    struct File* files;
    size_t files_n;
    struct Directory** dirs;
    size_t dirs_n;
    struct Directory* parent_dir;
    char name[16];
};

void dir_init(struct Directory* dir, char* name, struct Directory* parent)
{
    dir->files = calloc(64, sizeof(*dir->files));
    dir->dirs = calloc(64, sizeof(*dir->dirs));
    dir->files_n = 0;
    dir->dirs_n = 0;
    strncpy(dir->name, name, 16);
    dir->parent_dir = parent;
}

void dir_free(struct Directory* dir)
{
    for (size_t i = 0; i < dir->dirs_n; i++) {
        dir_free(dir->dirs[i]);
    }
    free(dir->files);
    free(dir->dirs);
    free(dir);
}

void dir_add_file(struct Directory* dir, struct File file)
{
    dir->files[dir->files_n] = file;
    dir->files_n++;
}

void dir_add_dir(struct Directory* dir, struct Directory* dir2)
{
    dir->dirs[dir->dirs_n] = dir2;
    dir->dirs_n++;
}

int32_t dir_size(struct Directory* dir)
{
    int32_t size = 0;
    for (size_t i = 0; i < dir->dirs_n; i++) {
        size += dir_size(dir->dirs[i]);
    }

    for (size_t i = 0; i < dir->files_n; i++) {
        size += dir->files[i].size;
    }

    return size;
}

struct Directory* dir_get_dir(struct Directory* dir, char* name)
{
    for (size_t i = 0; i < dir->dirs_n; i++) {
        if (strcmp(name, dir->dirs[i]->name) == 0) {
            return dir->dirs[i];
        }
    }

    return NULL;
}

void aggregate_size(struct Directory* dir, int32_t* agg, int32_t max_size)
{
    int32_t size = dir_size(dir);
    if (size < max_size) {
        *agg += size;
    }
    for (size_t i = 0; i < dir->dirs_n; i++) {
        aggregate_size(dir->dirs[i], agg, max_size);
    }
}

void dir_find_min_fit(struct Directory* dir, int32_t* cur_size, int32_t min_size)
{

    int32_t size = dir_size(dir);
    if (size > min_size && size < *cur_size) {
        *cur_size = size;
    }
    for (size_t i = 0; i < dir->dirs_n; i++) {
        dir_find_min_fit(dir->dirs[i], cur_size, min_size);
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
    char buffer[buffer_size];
    struct Directory* base_dir = malloc(sizeof(*base_dir));
    dir_init(base_dir, "/", NULL);
    struct Directory* cur_dir = base_dir;
    // ignore the first two lines
    fgets(buffer, buffer_size, fp);
    fgets(buffer, buffer_size, fp);

    while (fgets(buffer, buffer_size, fp)) {
        // replace newline with null terminator
        buffer[strcspn(buffer, "\n")] = 0;

        char next_dir_name[32];
        if (strncmp(buffer, "$ ls", 4) == 0) {
            // ignore ls command
        } else if (strncmp(buffer, "$ cd ..", 7) == 0) {
            // go to the parent directory
            cur_dir = cur_dir->parent_dir;
        } else if (sscanf(buffer, "$ cd %s", next_dir_name) == 1) {
            // go into the new directory and create it if necessary
            struct Directory* new_dir = dir_get_dir(cur_dir, next_dir_name);
            if (new_dir == NULL) {
                new_dir = malloc(sizeof(*new_dir));
                dir_init(new_dir, next_dir_name, cur_dir);
                dir_add_dir(cur_dir, new_dir);
            }
            cur_dir = new_dir;
        } else {
            // got files, parse and add to current directory
            // assume no duplicate files are ever looked at
            if (sscanf(buffer, "dir %s", next_dir_name) == 1) {
                // got directory
                // add it
                struct Directory* new_dir = dir_get_dir(cur_dir, next_dir_name);
                if (new_dir == NULL) {
                    new_dir = malloc(sizeof(*new_dir));
                    dir_init(new_dir, next_dir_name, cur_dir);
                    dir_add_dir(cur_dir, new_dir);
                }
            } else {
                // got file
                struct File file;
                // this should never fail
                if (sscanf(buffer, "%d %s", &file.size, file.name) != 2) {
                    fprintf(stderr, "could not parse line '%s'\n", buffer);
                    return 1;
                }

                dir_add_file(cur_dir, file);
            }
        }
    }

    int32_t agg = 0;
    aggregate_size(base_dir, &agg, 100000);
    printf("<100k sum %d\n", agg);

    int32_t min_size = INT32_MAX;
    dir_find_min_fit(base_dir, &min_size, 30000000 - (70000000 - dir_size(base_dir)));
    printf("min dir size %d\n", min_size);

    dir_free(base_dir);

    fclose(fp);
    return 0;
}

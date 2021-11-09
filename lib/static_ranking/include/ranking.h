//  Copyright 2021 Eremey Remzin
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include<sys/mman.h>

#define MAX_SEND_SIZE 248
#define MAX_NUMBER_STR_SIZE 10
#define RANG_TYPE 1

#define MAIN_PROCESS_ID 0

typedef struct ranked_file {
    int64_t rank;
    char file_name[MAX_SEND_SIZE];
} ranked_file;

typedef struct message_buf {
    int64_t mtype;
    char mtext[MAX_SEND_SIZE];
} message_buf;

size_t ranked_files_init(ranked_file** ranked_files, char* directory_path,
                      size_t* number_of_files);

int rank_files(ranked_file* ranked_files, const char *directory_path,
               const char *request, size_t number_of_files);

int get_top5(size_t top5_indexes[5], ranked_file* ranked_files,
             size_t number_of_files);

int print_top(const size_t top5_indexes[5], ranked_file* ranked_files,
              size_t number_of_files);

int free_ranked_files(ranked_file* ranked_files);

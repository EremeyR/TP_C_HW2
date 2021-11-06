#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>


#include <fcntl.h>
#include <sys/stat.h>
#include<sys/mman.h>

#define MAX_SEND_SIZE 80
#define MAX_NUMBER_STR_SIZE 10
#define RANG_TYPE 80

#define MAIN_PROCESS_ID 0

typedef struct message_buf {
    long mtype;
    char mtext[MAX_SEND_SIZE];
} message_buf;

int get_file_names(char* dir_name, char (*file_list)[256], size_t list_size);

int rank_files(size_t number_of_files, const char *directory_path, char (*file_list)[256], size_t* rank_list);

int get_top5(size_t* top5_indexes, const size_t* ranks, size_t size);

int print_top(const size_t* top5_indexes, const size_t* ranks, char (*file_list)[256]);
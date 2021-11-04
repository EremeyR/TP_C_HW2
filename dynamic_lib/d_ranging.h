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

#define MAX_SEND_SIZE 80
#define MAX_NUMBER_STR_SIZE 10
#define RANG_TYPE 80

#define MAIN_PROCESS_ID 0

typedef struct message_buf {
    long mtype;
    char mtext[MAX_SEND_SIZE];
} message_buf;

int message_parser(const char *message, size_t* position, size_t* rank);

int send_rank(int qid, message_buf *qbuf, long type, char *text);
int get_rank(size_t* position, size_t* rank, int qid, message_buf *qbuf, long type);

int check_type(char* file_name);
int get_file_names(char* dir_name, char** file_list, size_t list_size);

int rank(size_t* rank);
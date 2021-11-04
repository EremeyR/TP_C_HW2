#include "d_ranging.h"

int message_parser(const char *message, size_t* position, size_t* rank) {
    if (message == NULL || position == NULL || rank == NULL) {
        return -1;
    }
    if (strlen(message) < 3) {
        return -1;
    }

    char position_str[MAX_NUMBER_STR_SIZE + 1] = "";
    char rank_str[MAX_NUMBER_STR_SIZE + 1] = "";

    size_t i = 0;
    while (message[i] != '|') {
        if (i >=  MAX_NUMBER_STR_SIZE) {
            return -1;
        }

        position_str[i] = message[i];
        ++i;
    }
    position_str[i + 1] = '\0';

    size_t temporary_value = strtoul(position_str, NULL, 10);
    if (temporary_value == -1){
        return - 1;
    }
    *position = temporary_value;


    size_t j = i + 1;
    while (message[j] != '\0') {
        if (j - (i +  1) >=  MAX_NUMBER_STR_SIZE) {
            return -1;
        }

        rank_str[j - (i +  1)] = message[j];
        ++j;
    }

    temporary_value = strtoul(rank_str, NULL, 10);
    if (temporary_value == -1){
        return - 1;
    }
    *rank = temporary_value;

    return 0;
}

int send_rank(int qid, message_buf *qbuf, long type, char *text) {
    qbuf->mtype = type;
    strcpy(qbuf->mtext, text);
    if (msgsnd(qid, (struct msgbuf *)qbuf, strlen(qbuf->mtext) + 1, 0) == -1) {
        return -1;
    }
    return 0;
}

int get_rank(size_t* position, size_t* rank, int qid, message_buf *qbuf, long type) {
    if (qbuf == NULL || position == NULL || rank == NULL) {
        return -1;
    }

    qbuf->mtype = type;
    if (msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0) < 0) {
        return -1;
    } else {
        if (message_parser(qbuf->mtext, position, rank) == -1) {
            return -1;
        }
    }
    return 0;
}

int check_type(char* file_name) {
    if (strlen(file_name) == 0) {
        return -1;
    }

    char* text_format = ".txt";

    if (strlen(file_name) < strlen(text_format) + 1) {
        return 0;
    }

    size_t j = 0;
    for (size_t i = strlen(file_name) - strlen(text_format); i < strlen(file_name); ++i) {
        if(file_name[i] != text_format[j]) {
            return 0;
        }
        ++j;
    }
    return 1;
}

int get_file_names(char* dir_name, char** file_list, size_t list_size) {
    if (strlen(dir_name) == 0) {
        return -1;
    }
    if (file_list == NULL) {
        return -1;
    }
    if (list_size == 0) {
        return -1;
    }

    size_t number_of_text_files = 0;

    DIR *directory = opendir(dir_name);
    if (directory == NULL) {
        return -1;
    }
    struct dirent *file = NULL;

    while ((file = readdir(directory)) != NULL) {
        if(check_type(file->d_name) == -1) {
            return -1;
        }

        if(check_type(file->d_name) == 1) {
            ++number_of_text_files;
            if(number_of_text_files > list_size) {
                return -1;
            }

            file_list[number_of_text_files - 1] = malloc(file->d_reclen + 1);
            if(file_list[number_of_text_files - 1] == NULL) {
                return -1;
            }

            file_list[number_of_text_files - 1] = file->d_name;
        }
    }
    if(number_of_text_files != list_size) {
        return -1;
    }
    closedir(directory);
    return 0;
}

int rank(size_t* rank) {
    *rank = 15;
    return 0;
}
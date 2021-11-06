#include "d_ranging.h"

static int message_parser(const char *message, size_t* position, size_t* rank) {
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

static int get_path(char *path, size_t path_size, const char *directory_path, const char *file_name) {
    if (path == NULL || directory_path == NULL || file_name == NULL || path_size == 0){
        return -1;
    }

    if (strlen(file_name) == 0) {
        return -1;
    }

    if (strlen(path) != 0) {
        return -1;
    }

    strncat(path, directory_path, path_size);
    strncat(path, "/", path_size);
    strncat(path, file_name, path_size);

    if (strlen(path) != (strlen(file_name) + strlen(directory_path) + 1)) {
        return -1;
    }

    return 0;
}

static int add_processes(int* process_id, int* children_pid, size_t number_of_process) {
    for (int i = 0; i < number_of_process - 1; ++i) {
        int pid = fork();
        if(pid == -1) {
            return -1;
        }
        children_pid[i] = pid;
        if (pid == 0) {
            *process_id = i + 1;
            break;
        }
    }
    return 0;
}

static int close_processes(int process_id, int* children_pid, size_t number_of_process) {
    int stat = 0;

    if (process_id != MAIN_PROCESS_ID) {
        exit(0);
    } else {
        for (size_t i = 0; i < number_of_process - 1; ++i) {
            int status = waitpid(children_pid[i], &stat, 0);
            if (status == -1) {
                return -1;
            }
        }
    }
}

static int check_request(size_t* i, const char* text, const char* request) {
    if(text == NULL || request == NULL || strlen(request) == 0) {
        return -1;
    }

    for (size_t j = 0; j < strlen(request); ++j) {
        if (text[*i] == '\0') {
            return 0;
        }
        if (text[*i] != request[j]) {
            return 0;
        }
        ++(*i);
    }
    return 1;
}

static int rank_text(size_t* rank, const char* text, const char* request) {
    *rank = 0;

    size_t i = 0;
    do {
        if (text[i] == request[0]) {
            size_t result = check_request(&i, text, request);
            if (result == -1) {
                return -1;
            } else {
                *rank += result;
            }
        }
        ++i;
    } while (text[i] != '\0');
    return 0;
}

static int rank(size_t* rank, char* path, char* request) {
    int fd = open(path, O_RDONLY); // или O_RDWR
    struct stat st;
    stat(path, &st);

    size_t file_size = st.st_size;

    char *region = mmap(NULL,
                        file_size,
                        PROT_READ,
                        MAP_PRIVATE | MAP_POPULATE,
                        fd,
                        0);
    if (region == MAP_FAILED) {
        printf("mmap failed\n");
        close(fd);
        return 1;
    }
    if (rank_text(rank, region, request) == -1) {
        return -1;
    }

    //printf("%s: %zu\n", path,  i);

    if (munmap(region, file_size) != 0) {
        //printf("munmap failed\n");
    }

    close(fd);
    return 0;
}

static int get_rank(size_t* ranks, int qid, message_buf *qbuf, long type) {
    if (qbuf == NULL || ranks == NULL) {
        return -1;
    }

    qbuf->mtype = type;
    if (msgrcv(qid, (struct msgbuf *)qbuf, MAX_SEND_SIZE, type, 0) < 0) {
        return -1;
    } else {
        size_t position = 0;
        size_t rank = 0;
        if (message_parser(qbuf->mtext, &position, &rank) == -1) {
            return -1;
        } else {
            ranks[position] = rank;
        }
    }
    return 0;
}

static int send_rank(int qid, message_buf *qbuf, long type, char *text) {
    qbuf->mtype = type;
    strcpy(qbuf->mtext, text);
    if (msgsnd(qid, (struct msgbuf *)qbuf, strlen(qbuf->mtext) + 1, 0) == -1) {
        return -1;
    }
    return 0;
}

int rank_files(size_t number_of_files, const char *directory_path, char (*file_list)[256], size_t* rank_list) {
    int qtype = RANG_TYPE;
    int message_queue_id = 0;
    if ((message_queue_id = msgget(IPC_PRIVATE, IPC_CREAT|0660)) == -1) {
        return -1;
    }
    message_buf queue_buf = {0,""};

    size_t number_of_process = sysconf(_SC_NPROCESSORS_ONLN) - 1;
    int* children_pid = calloc(number_of_process - 1, sizeof(int));
    int process_id = MAIN_PROCESS_ID;

    add_processes(&process_id, children_pid, number_of_process);

    for (size_t i = process_id; i < number_of_files; i += number_of_process) {
        char message_text[MAX_SEND_SIZE] = "";

        char file_path[255] = "";
        if (get_path(file_path, 255, directory_path, file_list[i]) == -1) {
            return -1;
        }

        size_t file_rank = 0;
        rank(&file_rank, file_path, "t");
        sprintf(message_text, "%zu|%zu", i, file_rank);
        send_rank(message_queue_id, &queue_buf, qtype, message_text);
    }

    close_processes(process_id, children_pid, number_of_process);
    free(children_pid);

    for (int i = 0; i < number_of_files; ++i) {
        get_rank(rank_list, message_queue_id, &queue_buf, qtype);
    }

    for (int i = 0; i < number_of_files; ++i) {
        printf("%zu\n", rank_list[i]);
    }
}

static int check_type(char* file_name) {
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

int get_file_names(char* dir_name, char (*file_list)[256], size_t list_size) {
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

        int is_correct_type = check_type(file->d_name);
        if(is_correct_type == -1) {
            return -1;
        }

        if(is_correct_type == 1) {
            ++number_of_text_files;
            if(number_of_text_files > list_size) {
                return -1;
            }

            if(number_of_text_files == 18) {
                int i;
                i++;
            }

            if(file_list[number_of_text_files - 1] == NULL) {
                return -1;
            }

            strcpy(file_list[number_of_text_files - 1], file->d_name);     ////
        }
    }
    if(number_of_text_files != list_size) {
        return -1;
    }
    closedir(directory);
    return 0;
}

static int add_to_top(size_t position, size_t value, size_t* top5_indexes, size_t top_size) {
    if (position < top_size) {
        add_to_top(position + 1, top5_indexes[position], top5_indexes, top_size);
    }
    top5_indexes[position] = value;
    return 0;
}

int get_top5(size_t* top5_indexes, const size_t* ranks, size_t size) {
    if (ranks == NULL || size == 0) {
        return -1;
    }

    size_t top_size = 5;

    for (int i = 0; i < top_size; ++i) {
        top5_indexes[i] = -1;
    }

    for (int i = 0; i < size; ++i) {
        if(top5_indexes[top_size - 1] == -1 ||
        ranks[i] >= ranks[top5_indexes[top_size - 1]]) {

            int is_put = 0;
            int j = 0;
            while (j < top_size && !is_put){
                if(top5_indexes[j] == -1 || ranks[i] >= ranks[top5_indexes[j]]) {
                    add_to_top(j, i, top5_indexes, 5);
                    is_put = 1;
                }
                j++;
            }
        }
    }
    for (size_t i = 0; i < size; ++i) {
        //printf("pos: %zu, rnk: %zu\n", i, ranks[i]);
    }
    return 0;
}

int print_top(const size_t* top5_indexes, const size_t* ranks, char (*file_list)[256]) {
    printf("Top 5:\n\n");

    for (size_t i = 0; i < 5; ++i) {
        printf("%zu. %s, rank: %zu\n", i + 1, file_list[top5_indexes[i]], ranks[top5_indexes[i]]);
    }
}
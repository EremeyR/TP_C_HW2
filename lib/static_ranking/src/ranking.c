#include "ranking.h"

static int get_path(char *path, const char *directory_path, const char *file_name) {
    if (path == NULL || directory_path == NULL || file_name == NULL){
        return -1;
    }

    if (strlen(file_name) == 0) {
        return -1;
    }

    if (strlen(path) != 0) {
        return -1;
    }

    strncat(path, directory_path, 512);
    strncat(path, "/", 512);
    strncat(path, file_name, 512);

    if (strlen(path) != (strlen(file_name) + strlen(directory_path) + 1)) {
        return -1;
    }

    return 0;
}

static int check_request(size_t* i, const char* text, const char* request) {
    if(strlen(request) == 0) {
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
    --(*i);
    return 1;
}

static int rank_text(size_t* rank, const char* text, const char* request) {
    if (text[0] == '\n' || request[0] == '\n') {
        return -1;
    }

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

static int rank(size_t* rank, char* path, const char* request) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return -1;
    }

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
        close(fd);
        return 1;
    }
    if (rank_text(rank, region, request) == -1) {
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int rank_files(size_t number_of_files, const char *directory_path,
               char (*file_list)[256], size_t* rank_list, const char *request) {
    for (size_t i = 0; i < number_of_files; ++i) {

        char file_path[512] = "";
        if (get_path(file_path, directory_path, file_list[i]) == -1) {
            return -1;
        }

        size_t file_rank = 0;
        if (rank(&file_rank, file_path, request) == -1) {
            return -1;
        } else {
            rank_list[i] = file_rank;
        }
    }
    return 0;
}

static int check_type(const char* file_name) {
    if (file_name == NULL) {
        return -1;
    }

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
    if (file_list == NULL || dir_name == NULL) {
        return -1;
    }
    if (list_size == 0) {
        return -1;
    }
    if (strlen(dir_name) == 0) {
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
            closedir(directory);
            return -1;
        }

        if(is_correct_type == 1) {
            ++number_of_text_files;
            if(number_of_text_files > list_size) {
                closedir(directory);
                return -1;
            }

            if(file_list[number_of_text_files - 1] == NULL) {
                closedir(directory);
                return -1;
            }

            strcpy(file_list[number_of_text_files - 1], file->d_name);
        }
    }
    closedir(directory);
    if(number_of_text_files != list_size) {
        return -1;
    }
    return 0;
}

static int add_to_top(size_t position, size_t value, size_t* top5_indexes) {
    if (position < 5) {
        add_to_top(position + 1, top5_indexes[position], top5_indexes);
    }
    top5_indexes[position] = value;
    return 0;
}

int get_top5(size_t* top5_indexes, const size_t* ranks, size_t number_of_files) {
    if (ranks == NULL || number_of_files == 0 || top5_indexes == NULL) {
        return -1;
    }

    size_t top_size = 5;

    if (number_of_files < top_size) {
        return -1;
    }

    for (int i = 0; i < top_size; ++i) {
        top5_indexes[i] = -1;
    }

    for (int i = 0; i < number_of_files; ++i) {
        if (top5_indexes[top_size - 1] == -1 ||
            ranks[i] >= ranks[top5_indexes[top_size - 1]]) {

            int is_put = 0;
            int j = 0;
            while (j < top_size && !is_put){
                if(top5_indexes[j] == -1 || ranks[i] >= ranks[top5_indexes[j]]) {
                    add_to_top(j, i, top5_indexes);
                    is_put = 1;
                }
                j++;
            }
        }
    }
    return 0;
}

int print_top(const size_t* top5_indexes, const size_t* ranks, char (*file_list)[256]) {
    if (top5_indexes == NULL || ranks == NULL || file_list == NULL) {
        return -1;
    }
    printf("Top 5:\n\n");

    for (size_t i = 0; i < 5; ++i) {
        if (file_list[top5_indexes[i]] == NULL) {
            return -1;
        }
        printf("%zu. %s, rank: %zu\n", i + 1, file_list[top5_indexes[i]], ranks[top5_indexes[i]]);
    }
    return 0;
}

int get_top_from(char* directory_path, size_t number_of_files) {
    char (*file_list)[256];
    file_list = malloc(number_of_files * sizeof(char[256]));

    size_t* ranks;
    ranks = malloc(number_of_files * sizeof(size_t));

    size_t top5_indexes[5];

    if (get_file_names(directory_path, file_list, number_of_files) == -1) {
        free(file_list);
        free(ranks);
        return -1;
    }

    if (rank_files(number_of_files, directory_path, file_list, ranks, "sdf") == -1) {
        free(file_list);
        free(ranks);
        return -1;
    }

    if (get_top5(top5_indexes, ranks, number_of_files) == -1) {
        free(file_list);
        free(ranks);
        return -1;
    }

    if (print_top(top5_indexes, ranks, file_list) == -1) {
        free(file_list);
        free(ranks);
        return -1;
    }


    free(file_list);
    free(ranks);
    return 0;
}
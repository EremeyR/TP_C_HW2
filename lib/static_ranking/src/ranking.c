//  Copyright 2021 Eremey Remzin
#include "ranking.h"

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
    for (size_t i = strlen(file_name) - strlen(text_format);
         i < strlen(file_name); ++i) {
        if (file_name[i] != text_format[j]) {
            return 0;
        }
        ++j;
    }
    return 1;
}

size_t ranked_files_init(ranked_file** ranked_files, char* directory_path,
                         size_t* number_of_files) {
    if (ranked_files == NULL || directory_path == NULL) {
        return -1;
    }

    if (number_of_files == NULL) {
        return -1;
    }

    if (strlen(directory_path) == 0) {
        return -1;
    }

    *number_of_files = 0;
    *ranked_files = malloc(0);

    DIR *directory = opendir(directory_path);
    if (directory == NULL) {
        return -1;
    }
    struct dirent *file = NULL;

    while ((file = readdir(directory)) != NULL) {
        int is_correct_type = check_type(file->d_name);
        if (is_correct_type == -1) {
            closedir(directory);
            return -1;
        }

        if (is_correct_type == 1) {
            ++(*number_of_files);

            *ranked_files = realloc(*ranked_files,
                                    *number_of_files * sizeof(ranked_file));

            memcpy((*ranked_files)[(*number_of_files) - 1].file_name,
                   file->d_name, MAX_SEND_SIZE);
            (*ranked_files)[(*number_of_files) - 1].rank = 0;
        }
    }
    closedir(directory);
    return *number_of_files;
}

static int check_request(size_t* i, const char* text, const char* request) {
    if (strlen(request) == 0) {
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

static int rank_file(size_t* rank, char* path, const char* request) {
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

static int get_path(char *path, const char *directory_path,
                    const char *file_name) {
    if (path == NULL || file_name == NULL) {
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

int rank_files(ranked_file* ranked_files, const char *directory_path,
               const char *request, size_t number_of_files) {
    if (ranked_files == NULL || directory_path == NULL || request == NULL) {
        return -1;
    }

    for (size_t i = 0; i < number_of_files; ++i) {
        char file_path[512] = "";
        if (get_path(file_path, directory_path, ranked_files[i].file_name)
            == -1) {
            return -1;
        }

        size_t file_rank = 0;
        if (rank_file(&file_rank, file_path, request) == -1) {
            return -1;
        }
        ranked_files[i].rank = (int64_t)file_rank;
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

int get_top5(size_t top5_indexes[5], ranked_file* ranked_files,
             size_t number_of_files) {
    if (ranked_files == NULL || number_of_files == 0 || top5_indexes == NULL) {
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
            ranked_files[i].rank >= ranked_files[top5_indexes[top_size - 1]].rank) {
            int is_put = 0;
            int j = 0;
            while (j < top_size && !is_put) {
                if (top5_indexes[j] == -1 ||
                    ranked_files[i].rank >= ranked_files[top5_indexes[j]].rank) {
                    add_to_top(j, i, top5_indexes);
                    is_put = 1;
                }
                j++;
            }
        }
    }
    return 0;
}

int print_top(const size_t top5_indexes[5], ranked_file* ranked_files,
              size_t number_of_files) {
    if (top5_indexes == NULL || ranked_files == NULL) {
        return -1;
    }
    printf("Top 5 out of %zu files:\n\n", number_of_files);

    for (size_t i = 0; i < 5; ++i) {
        printf("%zu. %s, rank: %zu\n", i + 1,
               ranked_files[top5_indexes[i]].file_name,
               ranked_files[top5_indexes[i]].rank);
    }
    return 0;
}

int free_ranked_files(ranked_file* ranked_files) {
    if (ranked_files != NULL) {
        free(ranked_files);
    }
    return 0;
}

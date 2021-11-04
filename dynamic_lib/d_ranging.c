#include "d_ranging.h"

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

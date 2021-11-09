//  Copyright 2021 Eremey Remzin
#include "test_utiles.h"
static int check_type(const char* file_name) {
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

int free_directory(char path[200]) {
    DIR *directory = opendir(path);
    if (directory == NULL) {
        return -1;
    }
    char (*file_paths)[512] = malloc(0);
    size_t number_of_files = 0;
    struct dirent *file = NULL;

    while ((file = readdir(directory)) != NULL) {
        if (check_type(file->d_name) == 1) {
            ++number_of_files;
            file_paths = realloc(file_paths, number_of_files * sizeof(*file_paths));
            strcat(file_paths[number_of_files-1], path);
            strcat(file_paths[number_of_files-1], "/");
            strcat(file_paths[number_of_files-1], file->d_name);
        }
    }

    for (int i = 0; i < number_of_files; ++i) {
        if(remove(file_paths[i]) == -1){
            free(file_paths);
            return -1;
        }
    }
    free(file_paths);
}

static int add_1mb_file(char file_path[256], char word[10]) {
    FILE *f = fopen(file_path, "w");
    if (f == NULL) {
        return -1;
    }

    char text[1000000] = "";
    for (int i = 0; i < 1000000 / strlen(word) ; ++i) {
        strcat(text, word);
    }
    fprintf(f, "%s", text);
    fclose(f);
    return 0;
}

static int add_1mb_file_with_keywords(char file_path[256] , size_t number) {
    size_t free_bytes = 1000000;
    char word[10] = "Test";
    free_bytes -= number * strlen(word);
    FILE *f = fopen(file_path, "w");
    if (f == NULL) {
        return -1;
    }

    char text[1000000] = "";
    for (int i = 0; i < number ; ++i) {
        strcat(text, word);
    }

    char* noise = "-----";
    for (int i = 0; i < free_bytes / strlen(noise) ; ++i) {
        strcat(text, noise);
    }
    fprintf(f, "%s", text);
    fclose(f);
    return 0;
}

int add_250k_Test(char path[200]) {
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/1MB_filled_Test.txt");

    char keyword[10] = "Test";
    add_1mb_file(file_path, keyword);
    return 0;
}

int add_empty_file(char path[200]) {
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/empty_file.txt");
    FILE *f = fopen(file_path, "w");
    if (f == NULL) {
        return -1;
    }
    fprintf(f, "");
    fclose(f);
    return 0;
}


int add_filled_noise_file(char path[200]) {
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/1MB_filled_Noise.txt");

    char noise[10] = "-----";
    add_1mb_file(file_path, noise);

    return 0;
}

int add_filled_keyword_file_with_spaces(char path[200]) {
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/1MB_filled_Test_with_spaces.txt");

    char keyword[10] = "Test ";
    add_1mb_file(file_path, keyword);

    return 0;
}

int add_1MB_files(char path[200], size_t quantity) {
    if(quantity > 9){
        return -1;
    }
    char file_path[256] = "";
    strcat(file_path, path);
    strcat(file_path, "/1MB_kw_nubmer__.txt");
    for (size_t i = 0; i < quantity; ++i) {
        file_path[strlen(file_path) - (1 + strlen(".txt"))] = (char)i +'0';
        add_1mb_file_with_keywords(file_path, i + 1);
    }
    return 0;
}

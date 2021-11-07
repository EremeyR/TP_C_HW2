#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include<sys/mman.h>

int get_file_names(char* dir_name, char (*file_list)[256], size_t list_size);

int rank_files(size_t number_of_files, const char *directory_path,
               char (*file_list)[256], size_t* rank_list, const char *request);

int get_top5(size_t* top5_indexes, const size_t* ranks, size_t number_of_files);

int print_top(const size_t* top5_indexes, const size_t* ranks, char (*file_list)[256]);

int get_top_from(char* directory_path, size_t number_of_files);
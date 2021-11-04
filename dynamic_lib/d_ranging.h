#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int check_type(char* file_name);
int get_file_names(char* dir_name, char** file_list, size_t list_size);

//  Copyright 2021 Eremey Remzin
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "stdio.h"

int free_directory(char path[200]);

int add_250k_Test(char path[200]);

int add_empty_file(char path[200]);

int add_filled_noise_file(char path[200]);

int add_filled_keyword_file_with_spaces(char path[200]);

int add_1MB_files(char path[200], size_t quantity);

double getCPUTime();
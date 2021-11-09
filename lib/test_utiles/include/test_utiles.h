//  Copyright 2021 Eremey Remzin
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "stdio.h"

int free_directory(char path[200]);

int add_filled_keyword_file(char path[200]);

int add_filled_noise_file(char path[200]);

int add_filled_keyword_file_with_spaces(char path[200]);

int add_10_1MB_files(char path[200]);
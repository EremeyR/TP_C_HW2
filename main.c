#include "static_lib/ranging.h"
#include "dynamic_lib/d_ranging.h"

int main() {
    size_t number_of_files = 500;
    char (*file_list)[256];
    file_list = malloc(number_of_files * sizeof(char[256]));

    size_t* ranks;
    ranks = malloc(number_of_files * sizeof(size_t));

    size_t top5_indexes[5];
    char directory_path[256] = "../Tests";

    get_file_names(directory_path, file_list, number_of_files);

    rank_files(number_of_files, directory_path, file_list, ranks);

    get_top5(top5_indexes, ranks, number_of_files);

    print_top(top5_indexes, ranks, file_list);


    free(file_list);
    free(ranks);
    return 0;
}

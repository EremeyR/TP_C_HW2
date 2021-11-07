#include "d_ranking.h"

int main() {
    size_t number_of_files = 500;
    char (*file_list)[256];
    file_list = malloc(number_of_files * sizeof(char[256]));

    size_t* ranks;
    ranks = malloc(number_of_files * sizeof(size_t));

    size_t top5_indexes[5];
    char directory_path[256] = "../Tests";
    char request[256] = "es";

    if (get_file_names(directory_path, file_list, number_of_files) == -1) {
        free(file_list);
        free(ranks);
        return -1;
    }

    if (rank_files(number_of_files, directory_path, file_list, ranks, request) == -1) {
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

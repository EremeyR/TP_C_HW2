//  Copyright 2021 Eremey Remzin
#include "../lib/static_ranking/include/ranking.h"

int main() {
    char directory_path[256] = "../Tests";
    char request[256] = "es";

    ranked_file* ranked_files;
    size_t top5_indexes[5];
    size_t number_of_files = 0;

    if (ranked_files_init(&ranked_files, directory_path,
                          &number_of_files) == -1) {
        return -1;
    }

    if (rank_files(ranked_files, directory_path,
                   request, number_of_files) == -1) {
        free_ranked_files(ranked_files);
        return -1;
    }

    if (get_top5(top5_indexes, ranked_files, number_of_files) == -1) {
        free_ranked_files(ranked_files);
        return -1;
    }

    if (print_top(top5_indexes, ranked_files, number_of_files) == -1) {
        free_ranked_files(ranked_files);
        return -1;
    }

    free_ranked_files(ranked_files);

    return 0;
}

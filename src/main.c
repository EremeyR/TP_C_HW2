//  Copyright 2021 Eremey Remzin
#include "../lib/static_ranking/include/ranking.h"
#define BUF_SIZE 256

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return -1;
    }
    if (strlen(argv[1]) >= BUF_SIZE ||
            strlen(argv[2]) >= BUF_SIZE) {
        return -1;
    }

    char directory_path[BUF_SIZE] = "";
    char request[BUF_SIZE] = "";
    memcpy(directory_path, argv[1], BUF_SIZE);
    memcpy(request, argv[2], BUF_SIZE);

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

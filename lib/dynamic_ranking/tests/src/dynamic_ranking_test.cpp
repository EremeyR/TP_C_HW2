
#include <gtest/gtest.h>

extern "C" {
#include "d_ranking.h"
#include "../../test_utiles/include/test_utiles.h"
}

TEST(INCORRECT_INPUT, ranked_files_init_incorrect_inp) {
    char directory_path[256] = "../Tests";
    ranked_file* ranked_files;
    size_t number_of_files;

    EXPECT_EQ(ranked_files_init(&ranked_files, directory_path, &number_of_files), -1);
    EXPECT_EQ(ranked_files_init(NULL, NULL, NULL), -1);
    EXPECT_EQ(ranked_files_init(&ranked_files, directory_path, NULL), -1);
    EXPECT_EQ(ranked_files_init(&ranked_files, NULL, &number_of_files), -1);
    EXPECT_EQ(ranked_files_init(NULL, directory_path, &number_of_files), -1);
}

TEST(INCORRECT_INPUT, rank_files_incorrect_inp) {
    char directory_path[256] = "../Tests";
    char request[256] = "Test";
    ranked_file* ranked_files;
    size_t number_of_files;
    EXPECT_EQ(rank_files(NULL,NULL, NULL, 0), -1);
    EXPECT_EQ(rank_files(ranked_files, directory_path, request, 1), -1);
    //EXPECT_EQ(rank_files(ranked_files, directory_path, request, 0), -1);
    EXPECT_EQ(rank_files(ranked_files, directory_path, NULL, 0), -1);
    EXPECT_EQ(rank_files(ranked_files, NULL, request, 0), -1);
    EXPECT_EQ(rank_files(NULL, directory_path, request, 0), -1);
}

TEST(INCORRECT_INPUT, get_top5_incorrect_inp) {
    ranked_file* ranked_files;
    size_t top5_indexes[5];
    size_t number_of_files;
    //EXPECT_EQ(get_top5(top5_indexes, ranked_files, number_of_files), -1);
    EXPECT_EQ(get_top5(NULL, ranked_files, number_of_files), -1);
    EXPECT_EQ(get_top5(top5_indexes, NULL, number_of_files), -1);
    EXPECT_EQ(get_top5(top5_indexes, ranked_files, 0), -1);
}

TEST(INCORRECT_INPUT, print_top_incorrect_inp) {
    ranked_file* ranked_files;
    size_t top5_indexes[5];
    size_t number_of_files;
    EXPECT_EQ(print_top(NULL, NULL, 0), -1);
    EXPECT_EQ(print_top(NULL, ranked_files, number_of_files), -1);
    EXPECT_EQ(print_top(top5_indexes, NULL, number_of_files), -1);
    EXPECT_EQ(print_top(top5_indexes, ranked_files, 0), -1);
}

TEST(INCORRECT_INPUT, free_ranked_files_incorrect_inp) {
    EXPECT_EQ(free_ranked_files(NULL), 0);
}

TEST(TEST_WITH_FILES, one_file_test) {
    char directory_path[256] = ".";
    char request[256] = "Test";
    size_t top5_indexes[5];
    ranked_file* ranked_files;
    size_t number_of_files;

    free_directory(directory_path);
    add_250k_Test(directory_path);

    EXPECT_EQ(ranked_files_init(&ranked_files, directory_path, &number_of_files), 1);
    EXPECT_EQ(ranked_files[0].rank == 0, 1);
    EXPECT_EQ(number_of_files == 1, 1);
    EXPECT_EQ(rank_files(ranked_files, directory_path, request, number_of_files), 0);
    EXPECT_EQ(ranked_files[0].rank == 250, 1);
    EXPECT_EQ(get_top5(top5_indexes, ranked_files, number_of_files), -1);
    free_ranked_files(ranked_files);
    free_directory(directory_path);
}

TEST(TEST_WITH_FILES, one_empty_file_test) {
    char directory_path[256] = ".";
    char request[256] = "Test";
    size_t top5_indexes[5];
    ranked_file* ranked_files;
    size_t number_of_files;

    free_directory(directory_path);
    add_empty_file(directory_path);

    EXPECT_EQ(ranked_files_init(&ranked_files, directory_path, &number_of_files), 1);
    EXPECT_EQ(ranked_files[0].rank == 0, 1);
    EXPECT_EQ(number_of_files == 1, 1);
    EXPECT_EQ(rank_files(ranked_files, directory_path, request, number_of_files), 0);
    EXPECT_EQ(ranked_files[0].rank == 0, 1);
    EXPECT_EQ(get_top5(top5_indexes, ranked_files, number_of_files), -1);
    free_ranked_files(ranked_files);
}

TEST(TEST_WITH_FILES, four_files_test) {
    char directory_path[256] = ".";
    char request[256] = "Test";
    size_t top5_indexes[5];
    ranked_file* ranked_files;
    size_t number_of_files;

    size_t real_number_of_files = 4;

    free_directory(directory_path);
    add_1MB_files(directory_path, real_number_of_files);

    EXPECT_EQ(ranked_files_init(&ranked_files, directory_path, &number_of_files), real_number_of_files);
    EXPECT_EQ(ranked_files[0].rank == 0, 1);
    EXPECT_EQ(number_of_files == real_number_of_files, 1);
    EXPECT_EQ(rank_files(ranked_files, directory_path, request, number_of_files), 0);
    EXPECT_EQ(get_top5(top5_indexes, ranked_files, number_of_files), -1);
    EXPECT_EQ(print_top(top5_indexes, ranked_files, number_of_files), -1);
    free_ranked_files(ranked_files);
}

TEST(TEST_WITH_FILES, four_files_with_empty_file_test) {
    char directory_path[256] = ".";
    char request[256] = "Test";
    size_t top5_indexes[5];
    ranked_file* ranked_files;
    size_t number_of_files;

    size_t real_number_of_files = 5;

    add_empty_file(directory_path);

    EXPECT_EQ(ranked_files_init(&ranked_files, directory_path, &number_of_files), real_number_of_files);
    EXPECT_EQ(ranked_files[0].rank == 0, 1);
    EXPECT_EQ(number_of_files == real_number_of_files, 1);
    EXPECT_EQ(rank_files(ranked_files, directory_path, request, number_of_files), 0);
    EXPECT_EQ(get_top5(top5_indexes, ranked_files, number_of_files), 0);
    for (size_t i = 0; i < real_number_of_files; ++i) {
        EXPECT_EQ(ranked_files[top5_indexes[i]].rank
        == (real_number_of_files - (i + 1)) , 1);
    }
    EXPECT_EQ(print_top(top5_indexes, ranked_files, number_of_files), 0);
    free_ranked_files(ranked_files);
}

TEST(TEST_WITH_FILES, six_files_test) {
    char directory_path[256] = ".";
    char request[256] = "Test";
    size_t top5_indexes[5];
    ranked_file* ranked_files;
    size_t number_of_files;

    size_t real_number_of_files = 6;

    add_250k_Test(directory_path);

    EXPECT_EQ(ranked_files_init(&ranked_files, directory_path, &number_of_files), real_number_of_files);
    EXPECT_EQ(ranked_files[0].rank == 0, 1);
    EXPECT_EQ(number_of_files == real_number_of_files, 1);
    EXPECT_EQ(rank_files(ranked_files, directory_path, request, number_of_files), 0);
    EXPECT_EQ(get_top5(top5_indexes, ranked_files, number_of_files), 0);
    EXPECT_EQ(ranked_files[top5_indexes[0]].rank == 250 , 1);
    for (size_t i = 1; i < 5; ++i) {
        EXPECT_EQ(ranked_files[top5_indexes[i]].rank
                  == (real_number_of_files - (i + 1)) , 1);
    }
    EXPECT_EQ(print_top(top5_indexes, ranked_files, number_of_files), 0);
    free_ranked_files(ranked_files);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

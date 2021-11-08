#include <gtest/gtest.h>

extern "C" {
    #include "ranking.h"
}

TEST(GET_DATA_TESTS, Assert_1) {
    EXPECT_EQ(ranked_files_init(NULL,NULL, NULL), -1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
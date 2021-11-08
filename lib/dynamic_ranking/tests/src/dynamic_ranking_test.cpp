
#include <gtest/gtest.h>

extern "C" {
    #include "d_ranking.h"
}

TEST(GET_DATA_TESTS, Assert_1) {
    EXPECT_EQ(get_file_names(NULL, NULL, 0), -1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
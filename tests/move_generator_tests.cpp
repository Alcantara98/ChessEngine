#include <gtest/gtest.h>
#include "move_generator.h"
#include "piece.h"

/**
 *  @brief Test Pawn
 *  
 *  @details Pawn, no pieces infront of it, one possible move forward.
 */
TEST(PawnTest, NoPiecesInFront) {
    EXPECT_EQ(generatePawnMove(1), expected_output);  // Replace with actual function and expected output
}
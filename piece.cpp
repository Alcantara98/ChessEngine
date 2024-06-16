#include <iostream>
#include <array>
#include <string>
#include <unordered_set>
#include <vector>

class Piece
{
private:
    const int piece_value;
    const int position_value[8][8];
    const std::string piece_name;

protected:
    struct Position {
        int x;
        int y;

        // Used to compare input move.
        bool operator==(const Position& other) const { return x == other.x && y == other.y; }
    };

    struct PositionHash {
        std::size_t operator()(const Position& pos) const
        {
            return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1);
        }
    };

    Position position;
    std::unordered_set<Position, PositionHash> validMoves;
    int value;
    static int move_count;

    virtual void updatePieceValue() = 0;
    virtual void updateValidMoves() = 0;

public:
    void updatePosition(Position& new_position)
    {
        if(validMoves.find(new_position) != validMoves.end()) {
            std::cout << "Position found!" << std::endl;
        } else {
            std::cout << "Position not found!" << std::endl;
        }
    }

    int getValue() { return value; }

    std::string getPieceName() { return piece_name; }
};

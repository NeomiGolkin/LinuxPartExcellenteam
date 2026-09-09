#ifndef KING_H
#define KING_H

#include "Piece.h"
#include <cmath>
#include <cstdlib>

class King : public Piece {
public:
    King(PieceColor color) : Piece(color, color == PieceColor::WHITE ? 'K' : 'k') {}

    bool isValidMove(int srcX, int srcY, int destX, int destY, const Piece* board[8][8]) const override {
        int diffX = std::abs(destX - srcX);
        int diffY = std::abs(destY - srcY);

        // מלך זז משבצת אחת לכל כיוון
        return (diffX <= 1 && diffY <= 1 && (diffX != 0 || diffY != 0));
    }
};

#endif // KING_H
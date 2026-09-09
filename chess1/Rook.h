#ifndef ROOK_H
#define ROOK_H

#include "Piece.h"
#include <cmath>

class Rook : public Piece {
public:
    Rook(PieceColor color) : Piece(color, color == PieceColor::WHITE ? 'R' : 'r') {}

    bool isValidMove(int srcX, int srcY, int destX, int destY, const Piece* board[8][8]) const override {
        // צריח זז רק לאורך שורה או עמודה
        if (srcX != destX && srcY != destY) {
            return false;
        }

        // בדיקה האם יש כלים שחוסמים את הדרך
        int stepX = (destX == srcX) ? 0 : (destX > srcX ? 1 : -1);
        int stepY = (destY == srcY) ? 0 : (destY > srcY ? 1 : -1);

        int currX = srcX + stepX;
        int currY = srcY + stepY;

        while (currX != destX || currY != destY) {
            if (board[currX][currY] != nullptr) {
                return false; // יש כלי בדרך
            }
            currX += stepX;
            currY += stepY;
        }

        return true;
    }
};

#endif // ROOK_H
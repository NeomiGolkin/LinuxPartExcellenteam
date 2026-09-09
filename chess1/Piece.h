#ifndef PIECE_H
#define PIECE_H

#include <string>

enum class PieceColor {
    WHITE,
    BLACK,
    NONE
};

class Piece {
protected:
    PieceColor m_color;
    char m_symbol; // 'R', 'r', 'K', 'k', etc.

public:
    Piece(PieceColor color, char symbol) : m_color(color), m_symbol(symbol) {}
    virtual ~Piece() = default;

    PieceColor getColor() const { return m_color; }
    char getSymbol() const { return m_symbol; }

    // פונקציה אבסטרקטית לבדיקת חוקיות תנועה בסיסית (ללא התחשבות בשח למלך עדיין, או עם תלות)
    virtual bool isValidMove(int srcX, int srcY, int destX, int destY, const Piece* board[8][8]) const = 0;
};

#endif // PIECE_H
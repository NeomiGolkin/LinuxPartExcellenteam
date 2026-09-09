#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include "Rook.h"
#include "King.h"
#include <string>
#include <cctype>

class Board {
private:
    Piece* m_board[8][8];
    PieceColor m_turn;

    // פונקציית עזר ליצירת כלי לפי תו
    Piece* createPiece(char symbol) {
        switch (symbol) {
            case 'R': return new Rook(PieceColor::WHITE);
            case 'r': return new Rook(PieceColor::BLACK);
            case 'K': return new King(PieceColor::WHITE);
            case 'k': return new King(PieceColor::BLACK);
            // ניתן להוסיף כאן בהמשך את שאר הכלים (B, b, Q, q, N, n, P, p)
            default: return nullptr;
        }
    }

public:
    Board(const std::string& initialBoard) {
        m_turn = PieceColor::WHITE; // הלבן מתחיל תמיד בשחמט רגיל (או לפי הגדרת המנוע)
        int index = 0;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (index < initialBoard.length()) {
                    char c = initialBoard[index++];
                    if (c == '#') {
                        m_board[i][j] = nullptr;
                    } else {
                        m_board[i][j] = createPiece(c);
                    }
                } else {
                    m_board[i][j] = nullptr;
                }
            }
        }
    }

    ~Board() {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                delete m_board[i][j];
            }
        }
    }

    // הפונקציה המרכזית שמקבלת את המחרוזת מהמנוע הגרפי ומחזירה את קוד התגובה
    int makeMove(const std::string& moveStr) {
        if (moveStr.length() < 4) return 1; // ILLEGAL_MOVE בסיסי

        int srcY = moveStr[0] - 'a';
        int srcX = 7 - (moveStr[1] - '1');
        int destY = moveStr[2] - 'a';
        int destX = 7 - (moveStr[3] - '1');

        // 1. בדיקה האם יש כלי במקור
        if (m_board[srcX][srcY] == nullptr) {
            return 2; // NO_PIECE_AT_SOURCE
        }

        Piece* sourcePiece = m_board[srcX][srcY];

        // 2. בדיקה האם זה כלי של השחקן היריב
        if ((m_turn == PieceColor::WHITE && sourcePiece->getColor() != PieceColor::WHITE) ||
            (m_turn == PieceColor::BLACK && sourcePiece->getColor() != PieceColor::BLACK)) {
            return 3; // OPPONENT_PIECE
        }

        // 3. בדיקה האם במשבצת היעד יש כלי של אותו שחקן
        if (m_board[destX][destY] != nullptr && m_board[destX][destY]->getColor() == sourcePiece->getColor()) {
            return 4; // OWN_PIECE_AT_DEST
        }

        // 4. בדיקת חוקיות גיאומטרית של הכלי (פולימורפיזם!)
        if (!sourcePiece->isValidMove(srcX, srcY, destX, destY, (const Piece***)m_board)) {
            return 1; // ILLEGAL_MOVE
        }

        // 5. ביצוע התנועה בפועל (לצורך הסימולציה)
        Piece* tempDest = m_board[destX][destY];
        m_board[destX][destY] = sourcePiece;
        m_board[srcX][srcY] = nullptr;

        // כאן תתווסף בהמשך בדיקה האם המהלך משאיר את המלך בשח -> LEAVES_KING_IN_CHECK (קוד 5)
        // לצורך הדוגמה כרגע נאשר את התנועה:

        // ניקוי הכלי הישן אם נדרס
        delete tempDest;

        // החלפת תור
        m_turn = (m_turn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;

        return 0; // LEGAL_MOVE
    }
};

#endif // BOARD_H
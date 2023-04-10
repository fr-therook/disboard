#include "piece.h"

using namespace disboard;

Piece::Piece(librustdisboard::Color color, librustdisboard::Role role)
        : impl(librustdisboard::Piece{color, role}) {}

Piece::Piece()
        : impl(librustdisboard::piece_default()) {}

Color Piece::color() const { return impl.color; }

Role Piece::role() const { return impl.role; }

QString Piece::roleStr() const {
    switch (impl.role) {
        case Role::Pawn:
            return "P";
        case Role::Knight:
            return "N";
        case Role::Bishop:
            return "B";
        case Role::Rook:
            return "R";
        case Role::Queen:
            return "Q";
        case Role::King:
            return "K";
    }
    return "";
}

QString Piece::pieceStr() const {
    if (impl.color == Color::White) {
        return "w" + roleStr();
    }
    return "b" + roleStr();
}

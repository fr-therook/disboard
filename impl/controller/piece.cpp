#include "piece.h"

static disboard::Piece::Color fromColor(librustdisboard::Color color) {
    switch (color) {
        case librustdisboard::Color::Black:
            return disboard::Piece::Color::Black;
        case librustdisboard::Color::White:
            return disboard::Piece::Color::White;
    }
}

static disboard::Piece::Role fromRole(librustdisboard::Role role) {
    switch (role) {
        case librustdisboard::Role::Pawn:
            return disboard::Piece::Role::Pawn;
        case librustdisboard::Role::Knight:
            return disboard::Piece::Role::Knight;
        case librustdisboard::Role::Bishop:
            return disboard::Piece::Role::Bishop;
        case librustdisboard::Role::Rook:
            return disboard::Piece::Role::Rook;
        case librustdisboard::Role::Queen:
            return disboard::Piece::Role::Queen;
        case librustdisboard::Role::King:
            return disboard::Piece::Role::King;
    }
}

disboard::Piece::Piece(librustdisboard::Piece piece)
        : mColor(fromColor(piece.color)),
          mRole(fromRole(piece.role)) {}

QString disboard::Piece::roleStr() {
    switch (mRole) {
        case disboard::Piece::Role::Pawn:
            return "P";
        case disboard::Piece::Role::Knight:
            return "N";
        case disboard::Piece::Role::Bishop:
            return "B";
        case disboard::Piece::Role::Rook:
            return "R";
        case disboard::Piece::Role::Queen:
            return "Q";
        case disboard::Piece::Role::King:
            return "K";
    }
    return "";
}

QString disboard::Piece::pieceStr() {
    if (mColor == disboard::Piece::Color::White) {
        return "w" + roleStr();
    }
    return "b" + roleStr();
}

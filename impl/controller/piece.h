#ifndef DISBOARD_PIECE_H
#define DISBOARD_PIECE_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "librustdisboard/lib.h"

namespace disboard {
    class Piece {
    Q_GADGET
        QML_VALUE_TYPE(piece)
        Q_PROPERTY(Color color MEMBER mColor)
        Q_PROPERTY(Role role MEMBER mRole)
        Q_PROPERTY(QString roleStr READ roleStr)
        Q_PROPERTY(QString pieceStr READ pieceStr)

    public:
        enum Color {
            Black = 0,
            White = 1,
        };
        enum Role {
            Pawn = 1,
            Knight = 2,
            Bishop = 3,
            Rook = 4,
            Queen = 5,
            King = 6,
        };

        explicit Piece(librustdisboard::Piece piece);
        Piece(Color color, Role role) : mColor(color), mRole(role) {}
        Piece() : Piece(Color::Black, Role::Knight) {}

        QString roleStr();
        QString pieceStr();

        Color mColor;
        Role mRole;
    };
}


#endif //DISBOARD_PIECE_H

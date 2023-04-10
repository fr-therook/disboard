#ifndef DISBOARD_PIECE_H
#define DISBOARD_PIECE_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "librustdisboard/lib.h"

namespace disboard {
    using librustdisboard::Color;
    using librustdisboard::Role;

    class Piece {
    Q_GADGET
        QML_VALUE_TYPE(piece)
        Q_PROPERTY(Color color READ color)
        Q_PROPERTY(Role role READ role)
        Q_PROPERTY(QString roleStr READ roleStr)
        Q_PROPERTY(QString pieceStr READ pieceStr)
        QML_UNCREATABLE("Piece can only be created on Rust side")

    public:
        Piece(Color color, Role role);
        Piece();

        Color color() const;
        Role role() const;

        QString roleStr() const;
        QString pieceStr() const;

        friend class Disboard;

    private:
        explicit Piece(librustdisboard::Piece piece)
            : impl(std::move(piece)) {}

        librustdisboard::Piece impl;
    };
}


#endif //DISBOARD_PIECE_H

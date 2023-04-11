#ifndef DISBOARD_MOVE_H
#define DISBOARD_MOVE_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "librustdisboard/lib.h"

#include "square.h"
#include "piece.h"

namespace disboard {
    class Move {
        Q_GADGET
        QML_VALUE_TYPE(move)
        QML_UNCREATABLE("Move can only be created on Rust side")

    public:
        Move(const Move& rhs);
        Move& operator=(const Move& rhs);

        Square from() const;
        Square to() const;

        bool isPromotion() const;
        void setPromotion(Role role);

        bool isEnPassant() const;

        bool isCastle() const;
        Square castleRookFrom() const;
        Square castleRookTo() const;

        QString toString() const;

    friend class Disboard;

    private:
        explicit Move(rust::Box<librustdisboard::Move> move)
                : impl(std::move(move)) {}

        rust::Box<librustdisboard::Move> impl;
    };
}


#endif //DISBOARD_MOVE_H

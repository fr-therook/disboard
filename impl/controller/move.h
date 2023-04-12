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

        [[nodiscard]] Square from() const;
        [[nodiscard]] Square to() const;

        [[nodiscard]] bool isPromotion() const;
        void setPromotion(Role role);

        [[nodiscard]] bool isEnPassant() const;

        [[nodiscard]] bool isCastle() const;
        [[nodiscard]] Square castleRookFrom() const;
        [[nodiscard]] Square castleRookTo() const;

        [[nodiscard]] QString toString() const;

    friend class Disboard;

    private:
        explicit Move(rust::Box<librustdisboard::Move> move)
                : impl(std::move(move)) {}

        rust::Box<librustdisboard::Move> impl;
    };
}


#endif //DISBOARD_MOVE_H

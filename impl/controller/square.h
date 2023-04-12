#ifndef DISBOARD_SQUARE_H
#define DISBOARD_SQUARE_H

#include "librustdisboard/lib.h"

#include <QObject>
#include <QtQml/qqmlregistration.h>

namespace disboard {
    class Square {
    Q_GADGET
        QML_VALUE_TYPE(square)
        Q_PROPERTY(uint8_t index READ index)
        Q_PROPERTY(uint8_t file READ file)
        Q_PROPERTY(uint8_t rank READ rank)
    public:
        Square();
        Square(uint8_t file, uint8_t rank);

        [[nodiscard]] uint8_t index() const;
        [[nodiscard]] uint8_t file() const;
        [[nodiscard]] uint8_t rank() const;

        bool operator==(Square &rhs);

        friend class Disboard;
        friend class Move;

    private:
        explicit Square(librustdisboard::Square square)
                : impl(square) {}

        librustdisboard::Square impl;
    };
}


#endif //DISBOARD_SQUARE_H

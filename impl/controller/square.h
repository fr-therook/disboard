#ifndef DISBOARD_SQUARE_H
#define DISBOARD_SQUARE_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "librustdisboard/lib.h"

namespace disboard {
    class Square {
    Q_GADGET
        QML_VALUE_TYPE(square)
        Q_PROPERTY(uint8_t index READ index)
        Q_PROPERTY(uint8_t file READ file)
        Q_PROPERTY(uint8_t rank READ rank)
    public:
        Square() : impl(librustdisboard::square_default()) {}
        Square(uint8_t file, uint8_t rank)
                : impl(librustdisboard::square_from_coords(file, rank)) {}

        uint8_t index() const { return impl.index; }
        uint8_t file() const { return impl.file(); }
        uint8_t rank() const { return impl.rank(); }

        bool operator==(Square &rhs) {
            return impl == rhs.impl;
        }

        friend class Disboard;
        friend class Move;

    private:
        explicit Square(librustdisboard::Square square)
                : impl(std::move(square)) {}

        librustdisboard::Square impl;
    };
}


#endif //DISBOARD_SQUARE_H

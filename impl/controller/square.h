#ifndef DISBOARD_SQUARE_H
#define DISBOARD_SQUARE_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "librustdisboard/lib.h"

namespace disboard {
    class Square {
    Q_GADGET
        QML_VALUE_TYPE(square)
        Q_PROPERTY(uint8_t index MEMBER mIndex)
        Q_PROPERTY(uint8_t file READ file)
        Q_PROPERTY(uint8_t rank READ rank)
    public:
        explicit Square(uint8_t index)
                : mIndex(index) {}
        explicit Square(librustdisboard::Square square)
            : Square(square.index) {}
        Square() : Square(0) {}

        uint8_t index() { return mIndex; }
        uint8_t file() { return mIndex & 7; }
        uint8_t rank() { return mIndex >> 3; }

        bool operator==(Square& rhs) {
            return this->index() == rhs.index();
        }

    private:
        uint8_t mIndex;
    };
}


#endif //DISBOARD_SQUARE_H

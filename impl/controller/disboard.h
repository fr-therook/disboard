#ifndef DISBOARD_DISBOARD_H
#define DISBOARD_DISBOARD_H

#include "librustdisboard/lib.h"

#include "square.h"
#include "piece.h"

#include <QUuid>

namespace disboard {
    class Disboard {
    public:
        Disboard();

        QUuid root();

        bool whites_turn(QUuid node);

        std::tuple<QVector<disboard::Square>, QVector<disboard::Piece>> pieces(QUuid node);
        std::optional<disboard::Piece> pieceAt(QUuid node, disboard::Square square);
        std::optional<rust::Box<librustdisboard::Move>>
            legalMove(QUuid node, disboard::Square from, disboard::Square to);

        std::optional<rust::Box<librustdisboard::Move>>
            lastMove(QUuid node);

        std::tuple<QVector<disboard::Square>, QVector<disboard::Square>>
            hints(QUuid node, disboard::Square from);

        std::optional<QUuid> prevNode(QUuid node);
        std::optional<QUuid> nextMainlineNode(QUuid node);

        QUuid addNode(QUuid node, rust::Box<librustdisboard::Move> move);

        QString pgn();

    private:
        rust::Box<librustdisboard::GameTree> tree;
    };
}


#endif //DISBOARD_DISBOARD_H

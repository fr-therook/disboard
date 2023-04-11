#ifndef DISBOARD_DISBOARD_H
#define DISBOARD_DISBOARD_H

#include "librustdisboard/lib.h"

#include "square.h"
#include "piece.h"
#include "move.h"

#include <QUuid>

namespace disboard {
    class Disboard {
    public:
        Disboard();

        QUuid root() const;

        Color turn(QUuid node) const;

        std::tuple<QVector<Square>, QVector<Piece>> pieces(QUuid node) const;
        std::optional<Piece> pieceAt(QUuid node, Square square) const;
        std::optional<Move> legalMove(QUuid node, Square from, Square to) const;

        std::optional<Move> lastMove(QUuid node) const;

        std::tuple<QVector<Square>, QVector<Square>>
            hints(QUuid node, Square from) const;

        std::optional<QUuid> prevNode(QUuid node) const;
        std::optional<QUuid> nextMainlineNode(QUuid node) const;

        QVector<QUuid> siblings(QUuid node) const;
        QVector<QUuid> mainlineNodes(QUuid node) const;

        QUuid addNode(QUuid node, Move move);

        QString pgn() const;

    private:
        rust::Box<librustdisboard::GameTree> tree;
    };
}


#endif //DISBOARD_DISBOARD_H

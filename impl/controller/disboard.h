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

        [[nodiscard]] QUuid root() const;

        [[nodiscard]] Color turn(QUuid node) const;

        [[nodiscard]] std::tuple<QVector<Square>, QVector<Piece>> pieces(QUuid node) const;
        [[nodiscard]] std::optional<Piece> pieceAt(QUuid node, Square square) const;
        [[nodiscard]] std::optional<Move> legalMove(QUuid node, Square from, Square to) const;

        [[nodiscard]] std::optional<Move> lastMove(QUuid node) const;

        [[nodiscard]] std::tuple<QVector<Square>, QVector<Square>>
            hints(QUuid node, Square from) const;

        [[nodiscard]] std::optional<QUuid> prevNode(QUuid node) const;
        [[nodiscard]] std::optional<QUuid> nextMainlineNode(QUuid node) const;

        [[nodiscard]] QVector<QUuid> siblings(QUuid node) const;
        [[nodiscard]] QVector<QUuid> mainlineNodes(QUuid node) const;

        QUuid addNode(QUuid node, Move move);

        [[nodiscard]] QString pgn() const;

    private:
        rust::Box<librustdisboard::GameTree> tree;
    };
}


#endif //DISBOARD_DISBOARD_H

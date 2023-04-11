#include "disboard.h"

#include <QDebug>

using namespace disboard;

QUuid from_uuid(librustdisboard::Uuid uuid) {
    return QUuid{
            uuid.l, uuid.w1, uuid.w2,
            uuid.b[0], uuid.b[1], uuid.b[2], uuid.b[3],
            uuid.b[4], uuid.b[5], uuid.b[6], uuid.b[7]
    };
}

librustdisboard::Uuid from_quuid(QUuid uuid) {
    std::array<uint8_t, 8> b = reinterpret_cast<const std::array<unsigned char, 8> &>(uuid.data4);
    return librustdisboard::Uuid{
            uuid.data1,
            uuid.data2,
            uuid.data3,
            b
    };
}

Disboard::Disboard()
    : tree(librustdisboard::game_default()) {
    qDebug() << "initializing disboard ffi";
}

QUuid Disboard::root() const {
    return from_uuid(tree->root());
}

Color Disboard::turn(QUuid node) const {
    return tree->position(from_quuid(node))->turn();
}

std::tuple<QVector<Square>, QVector<Piece>>
Disboard::pieces(QUuid node) const {
    auto position = tree->position(from_quuid(node));
    auto _squares = position->squares();
    auto _pieces = position->pieces();

    QVector<Square> squares;
    QVector<Piece> pieces;
    for (auto square: _squares) {
        squares.push_back(Square{square});
    }
    for (auto piece: _pieces) {
        pieces.push_back(Piece{piece});
    }

    return std::make_tuple(squares, pieces);
}

std::optional<Piece> Disboard::pieceAt(QUuid node, Square square) const {
    auto position = tree->position(from_quuid(node));
    if (position->has_piece_at(square.impl)) {
        auto piece = position->piece_at(square.impl);
        return Piece(piece);
    }
    return {};
}

std::optional<Move>
Disboard::legalMove(QUuid node, Square from, Square to) const {
    auto position = tree->position(from_quuid(node));
    if (position->has_legal_move(from.impl, to.impl)) {
        return Move{
            position->legal_move(from.impl, to.impl)
        };
    }
    return {};
}

std::optional<Move>
Disboard::lastMove(QUuid node) const {
    auto _node = from_quuid(node);
    if (tree->has_prev_move(_node)) {
        return Move{
            tree->prev_move(_node)
        };
}
    return {};
}

std::tuple<QVector<Square>, QVector<Square>>
Disboard::hints(QUuid node, Square from) const {
    auto position = tree->position(from_quuid(node));

    auto hint_vec = position->hints(from.impl);
    auto capture_vec = position->captures(from.impl);

    QVector<Square> hints, captures;
    for (auto square: hint_vec) {
        hints.emplace_back(Square{square});
    }
    for (auto square: capture_vec) {
        captures.emplace_back(Square{square});
    }

    return std::make_tuple(hints, captures);
}

std::optional<QUuid> Disboard::prevNode(QUuid node) const {
    if (!tree->has_prev_node(from_quuid(node))) return {};
    return from_uuid(tree->prev_node(from_quuid(node)));
}

std::optional<QUuid> Disboard::nextMainlineNode(QUuid node) const {
    if (!tree->has_next_mainline_node(from_quuid(node))) return {};
    return from_uuid(tree->next_mainline_node(from_quuid(node)));
}

QVector<QUuid> Disboard::mainlineNodes(QUuid node) const {
    auto node_vec = tree->mainline_nodes(from_quuid(node));
    QVector<QUuid> nodes;
    for (auto _node : node_vec) {
        nodes.push_back(from_uuid(_node));
    }
    return nodes;
}

QUuid Disboard::addNode(QUuid node, Move move) {
    auto new_node = tree->add_node(
            from_quuid(node),
            std::move(move.impl)
            );
    return from_uuid(new_node);
}

QString Disboard::pgn() const {
    auto pgn = tree->pgn();
    std::string pgnStr{pgn};
    return QString::fromStdString(pgnStr);
}

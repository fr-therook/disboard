#include "disboard.h"

#include <QDebug>

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

disboard::Disboard::Disboard() : tree(librustdisboard::new_game()) {
    qDebug() << "initializing disboard ffi";
}

QUuid disboard::Disboard::root() {
    return from_uuid(tree->root());
}

bool disboard::Disboard::whites_turn(QUuid node) {
    auto position = tree->position(from_quuid(node));
    return position->turn() == librustdisboard::Color::White;
}

std::tuple<QVector<disboard::Square>, QVector<disboard::Piece>>
disboard::Disboard::pieces(QUuid node) {
    auto position = tree->position(from_quuid(node));
    auto _squares = position->squares();
    auto _pieces = position->pieces();

    QVector<disboard::Square> squares;
    QVector<disboard::Piece> pieces;
    for (auto square: _squares) {
        squares.emplace_back(square);
    }
    for (auto piece: _pieces) {
        pieces.emplace_back(piece);
    }

    return std::make_tuple(squares, pieces);
}

std::optional<disboard::Piece> disboard::Disboard::pieceAt(QUuid node, disboard::Square square) {
    auto position = tree->position(from_quuid(node));
    librustdisboard::Square _square{square.index()};
    if (position->has_piece_at(_square)) {
        auto piece = position->piece_at(_square);
        return disboard::Piece(piece);
    }
    return {};
}

std::optional<rust::Box<librustdisboard::Move>>
disboard::Disboard::legalMove(QUuid node, disboard::Square from, disboard::Square to) {
    auto position = tree->position(from_quuid(node));
    librustdisboard::Square src_sq{from.index()};
    librustdisboard::Square dest_sq{to.index()};
    if (position->has_legal_move(src_sq, dest_sq)) {
        return position->legal_move(src_sq, dest_sq);
    }
    return {};
}

std::optional<rust::Box<librustdisboard::Move>>
disboard::Disboard::lastMove(QUuid node) {
    auto _node = from_quuid(node);
    if (tree->has_prev_move(_node)) {
        return tree->prev_move(_node);
    }
    return {};
}

std::tuple<QVector<disboard::Square>, QVector<disboard::Square>>
disboard::Disboard::hints(QUuid node, disboard::Square from) {
    auto position = tree->position(from_quuid(node));
    librustdisboard::Square _from{from.index()};

    auto hint_vec = position->hints(_from);
    auto capture_vec = position->captures(_from);

    QVector<disboard::Square> hints, captures;
    for (auto square: hint_vec) {
        hints.emplace_back(square);
    }
    for (auto square: capture_vec) {
        captures.emplace_back(square);
    }

    return std::make_tuple(hints, captures);
}

std::optional<QUuid> disboard::Disboard::prevNode(QUuid node) {
    if (!tree->has_prev_node(from_quuid(node))) return {};
    return from_uuid(tree->prev_node(from_quuid(node)));
}

std::optional<QUuid> disboard::Disboard::nextMainlineNode(QUuid node) {
    if (!tree->has_next_mainline_node(from_quuid(node))) return {};
    return from_uuid(tree->next_mainline_node(from_quuid(node)));
}

QUuid disboard::Disboard::addNode(QUuid node, rust::Box<librustdisboard::Move> move) {
    auto new_node = tree->add_node(from_quuid(node), std::move(move));
    return from_uuid(new_node);
}

QString disboard::Disboard::pgn() {
    auto pgn = tree->pgn();
    std::string pgnStr{pgn};
    return QString::fromStdString(pgnStr);
}

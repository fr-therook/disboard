#include "controller.h"

#include <QDebug>

#include <optional>
#include <tuple>

#include "disboard.h"

uint8_t coord_to_square(float x, float y, int piece_size) {
    uint32_t file = ((int) x) / piece_size;
    uint32_t rank = 7 - ((int) y) / piece_size;

    return (rank << 3) | file;
}

class Controller::p {
    friend Controller;
public:
    p(Controller *q) : q(q), board({}), curNode(board.root()) {}

    void resync() {
        QVector<disboard::Square> squares;
        QVector<disboard::Piece> pieces;
        std::tie(squares, pieces) = board.pieces(curNode);
        emit q->resetBoard(squares, pieces);
    }

    void clicked(uint8_t sq_index) {
        auto _highlightedSq = highlightedSq;
        highlightedSq.reset();

        if (cancelPromotion()) return;

        auto sq = disboard::Square(sq_index);

        if (_highlightedSq.has_value()) {
            auto srcSq = *_highlightedSq;
            if (srcSq == sq) {
                // Clicked the same square twice
                return;
            }

            if (auto m = board.legalMove(curNode, srcSq, sq)) {
                // A legal move!
                emit q->movePiece(srcSq, sq);

                tryApplyMove(std::move(*m));
                return;
            }
        }

        if (!board.pieceAt(curNode, sq).has_value()) {
            // No new piece selected
            return;
        }

        highlightedSq = sq;
    }

    void dragStarted(uint8_t sq_index) {
        highlightedSq.reset();

        if (cancelPromotion()) return;

        auto sq = disboard::Square(sq_index);

        auto piece = board.pieceAt(curNode, sq);
        if (!piece.has_value()) {
            // No new piece selected
            return;
        }

        highlightedSq = sq;

        dragged.emplace(
                sq,
                *piece
        );
        emit q->dragChanged();

        emit q->removePiece(sq);
    }

    void dragEnded(uint8_t sq_index) {
        auto _dragged = dragged;
        dragged = {};
        if (!_dragged.has_value()) return;

        auto destSq = disboard::Square(sq_index);
        auto srcSq = _dragged->square;
        auto piece = _dragged->piece;

        auto m = board.legalMove(curNode, srcSq, destSq);
        if (!m.has_value()) {
            emit q->placePiece(piece, srcSq);
            return;
        }

        // A legal move
        highlightedSq = {};
        emit q->highlightedSqChanged();

        emit q->placePiece(piece, destSq);

        tryApplyMove(std::move(*m));
    }

    void promote(disboard::Piece piece) {
        auto _promotion = std::move(promotion);
        promotion = {};
        if (!_promotion.has_value()) {
            qDebug() << "Nothing to promote...";
            return;
        }
        librustdisboard::Role _role;
        switch (piece.mRole) {
            case disboard::Piece::Role::Knight:
                _role = librustdisboard::Role::Knight;
                break;
            case disboard::Piece::Role::Rook:
                _role = librustdisboard::Role::Rook;
                break;
            case disboard::Piece::Role::Bishop:
                _role = librustdisboard::Role::Bishop;
                break;
            default:
                _role = librustdisboard::Role::Queen;
                break;
        }
        (*_promotion)->set_promotion(_role);

        emit q->placePiece(piece, disboard::Square((*_promotion)->to()));

        applyMove(std::move(*_promotion));
    }

private:
    Controller *q;
    disboard::Disboard board;

    int pieceSize;
    QUuid curNode;
    std::optional<disboard::Square> highlightedSq;

    struct DraggedPiece {
        disboard::Square square;
        disboard::Piece piece;

        DraggedPiece(disboard::Square square, disboard::Piece piece)
                : square(square), piece(piece) {}
    };

    std::optional<DraggedPiece> dragged;
    std::optional<rust::Box<librustdisboard::Move>> promotion;

    void tryApplyMove(rust::Box<librustdisboard::Move> m) {
        if (m->is_promotion()) {
            promotion.emplace(
                    std::move(m)
            );
            emit q->promotionChanged();

            return;
        }

        if (m->is_en_passant()) {
            auto epSq = disboard::Square(m->from().rank() << 3 | m->to().file());
            emit q->removePiece(epSq);
        }

        if (m->is_castle()) {
            // move the rook
            emit q->movePiece(
                    disboard::Square(m->castle_rook_from()),
                    disboard::Square(m->castle_rook_to())
            );
        }

        applyMove(std::move(m));
    }

    void applyMove(rust::Box<librustdisboard::Move> m) {
        auto newNode = board.addNode(curNode, std::move(m));
        q->setCurNode(newNode);
        emit q->treeChanged();
    }

    bool cancelPromotion() {
        auto _promotion = std::move(promotion);
        promotion = {};
        if (!_promotion.has_value()) return false;

        emit q->promotionChanged();

        auto fromSq = disboard::Square((*_promotion)->from());
        auto toSq = disboard::Square((*_promotion)->to());
        emit q->movePiece(toSq, fromSq);

        if (auto captured = board.pieceAt(curNode, toSq)) {
            emit q->placePiece(*captured, toSq);
        }

        return true;
    }
};

Controller::Controller(QObject *parent)
        : QObject(parent),
          p(new class Controller::p(this)) {}

void Controller::resyncBoard() {
    p->resync();
}

void Controller::coordClicked(float x, float y) {
    auto sq_index = coord_to_square(x, y, pieceSize());
    p->clicked(sq_index);
    emit highlightedSqChanged();
}

void Controller::coordDragStarted(
        float startX, float startY,
        float endX, float endY
) {
    auto sq_index = coord_to_square(startX, startY, pieceSize());
    p->dragStarted(sq_index);
    emit highlightedSqChanged();
}

void Controller::coordDragEnded(
        float startX, float startY,
        float endX, float endY
) {
    auto sq_index = coord_to_square(endX, endY, pieceSize());
    p->dragEnded(sq_index);
    emit dragChanged();
}

void Controller::promote(disboard::Piece piece) {
    p->promote(piece);
    emit promotionChanged();
}

void Controller::prevMove() {
    auto prevNode = p->board.prevNode(curNode());
    if (!prevNode.has_value()) return;

    setCurNode(*prevNode);
    resyncBoard();
}

void Controller::nextMove() {
    auto nextNode = p->board.nextMainlineNode(curNode());
    if (!nextNode.has_value()) return;

    setCurNode(*nextNode);
    resyncBoard();
}

int Controller::pieceSize() {
    return p->pieceSize;
}

void Controller::setPieceSize(int newValue) {
    if (p->pieceSize == newValue) {
        return;
    }
    p->pieceSize = newValue;
    emit curNodeChanged();
}

QUuid Controller::curNode() {
    return p->curNode;
}

void Controller::setCurNode(QUuid newValue) {
    if (p->curNode == newValue) {
        return;
    }
    p->curNode = newValue;
    emit curNodeChanged();
}

QVariant Controller::promotionSq() {
    if (!p->promotion.has_value()) return {};

    auto sq = (*p->promotion)->to();
    return QVariant::fromValue(disboard::Square(sq));
}

QVariant Controller::promotionPieces() {
    if (!p->promotion.has_value()) return {};

    auto sq = (*p->promotion)->to();
    auto color = sq.rank() >= 4 ?
                 disboard::Piece::Color::White : disboard::Piece::Color::Black;
    auto queen = disboard::Piece(
            color,
            disboard::Piece::Role::Queen
    );
    auto knight = disboard::Piece(
            color,
            disboard::Piece::Role::Knight
    );
    auto rook = disboard::Piece(
            color,
            disboard::Piece::Role::Rook
    );
    auto bishop = disboard::Piece(
            color,
            disboard::Piece::Role::Bishop
    );

    auto vec = QVector<disboard::Piece>{queen, knight, rook, bishop};

    return QVariant::fromValue(vec);
}

QVariant Controller::phantom() {
    if (!p->dragged.has_value()) return {};

    auto piece = p->dragged->piece;
    return QVariant::fromValue(piece);
}

QPointF Controller::dragPos() {
    return mDragPos;
}

void Controller::setDragPos(QPointF newValue) {
    if (mDragPos == newValue) return;
    mDragPos = newValue;
    emit dragPosChanged();
}

disboard::Square Controller::dragSq() {
    return disboard::Square(
            coord_to_square(
                    mDragPos.x(), mDragPos.y(),
                    std::max(pieceSize(), 1)
            )
    );
}

QVariant Controller::highlightedSq() {
    if (auto val = p->highlightedSq) {
        return QVariant::fromValue(*val);
    }
    return {};
}

QVariant Controller::lastSrcSq() {
    if (auto val = p->board.lastMove(curNode())) {
        return QVariant::fromValue(
                disboard::Square((*val)->from())
        );
    }
    return {};
}

QVariant Controller::lastDestSq() {
    if (auto val = p->board.lastMove(curNode())) {
        return QVariant::fromValue(
                disboard::Square((*val)->to())
        );
    }
    return {};
}

QVector<disboard::Square> Controller::hintSq() {
    if (auto _highlightSq = p->highlightedSq) {
        const auto [hints, _] = p->board.hints(curNode(), *_highlightSq);
        return hints;
    }
    return {};
}

QVector<disboard::Square> Controller::captureSq() {
    if (auto _highlightSq = p->highlightedSq) {
        const auto [_, captures] = p->board.hints(curNode(), *_highlightSq);
        return captures;
    }
    return {};
}

QString Controller::pgn() {
    return p->board.pgn();
}

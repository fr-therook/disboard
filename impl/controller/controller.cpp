#include "controller.h"

#include <optional>
#include <tuple>

disboard::Square coord_to_square(float x, float y, int piece_size) {
    uint8_t file = ((int) x) / piece_size;
    uint8_t rank = 7 - ((int) y) / piece_size;

    return disboard::Square{file, rank};
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

    void clicked(disboard::Square sq) {
        auto _highlightedSq = highlightedSq;
        highlightedSq.reset();

        if (cancelPromotion()) return;

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

    void dragStarted(disboard::Square sq) {
        highlightedSq.reset();

        if (cancelPromotion()) return;

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

    void dragEnded(disboard::Square destSq) {
        auto _dragged = dragged;
        dragged = {};
        if (!_dragged.has_value()) return;

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
        promotion = std::nullopt;
        if (!_promotion.has_value()) {
            qDebug() << "Nothing to promote...";
            return;
        }
        disboard::Role _role;
        switch (piece.role()) {
            case disboard::Role::Knight:
                _role = disboard::Role::Knight;
                break;
            case disboard::Role::Rook:
                _role = disboard::Role::Rook;
                break;
            case disboard::Role::Bishop:
                _role = disboard::Role::Bishop;
                break;
            default:
                _role = disboard::Role::Queen;
                break;
        }
        (*_promotion).setPromotion(_role);

        emit q->placePiece(piece, (*_promotion).to());

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
    std::optional<disboard::Move> promotion;

    void tryApplyMove(disboard::Move m) {
        if (m.isPromotion()) {
            promotion.emplace(
                    std::move(m)
            );
            emit q->promotionChanged();

            return;
        }

        if (m.isEnPassant()) {
            auto epSq = disboard::Square{m.to().file(), m.from().rank()};
            emit q->removePiece(epSq);
        }

        if (m.isCastle()) {
            // move the rook
            emit q->movePiece(
                    disboard::Square(m.castleRookFrom()),
                    disboard::Square(m.castleRookTo())
            );
        }

        applyMove(std::move(m));
    }

    void applyMove(disboard::Move m) {
        auto newNode = board.addNode(curNode, std::move(m));
        q->setCurNode(newNode);
        emit q->nodePushed(newNode);
        emit q->treeChanged();
    }

    bool cancelPromotion() {
        auto _promotion = std::move(promotion);
        promotion = std::nullopt;
        if (!_promotion.has_value()) return false;

        emit q->promotionChanged();

        auto fromSq = (*_promotion).from();
        auto toSq = (*_promotion).to();
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
    p->clicked(coord_to_square(x, y, pieceSize()));
    emit highlightedSqChanged();
}

void Controller::coordDragStarted(
        float startX, float startY,
        float endX, float endY
) {
    p->dragStarted(coord_to_square(startX, startY, pieceSize()));
    emit highlightedSqChanged();
}

void Controller::coordDragEnded(
        float startX, float startY,
        float endX, float endY
) {
    p->dragEnded(coord_to_square(endX, endY, pieceSize()));
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

int Controller::pieceSize() const {
    return p->pieceSize;
}

void Controller::setPieceSize(int newValue) {
    if (p->pieceSize == newValue) {
        return;
    }
    p->pieceSize = newValue;
    emit curNodeChanged();
}

QUuid Controller::root() const {
    return p->board.root();
}

QUuid Controller::curNode() const {
    return p->curNode;
}

void Controller::setCurNode(QUuid newValue) {
    if (p->curNode == newValue) {
        return;
    }
    p->curNode = newValue;
    emit curNodeChanged();
}

QVariant Controller::promotionSq() const {
    if (!p->promotion.has_value()) return {};

    auto sq = (*p->promotion).to();
    return QVariant::fromValue(sq);
}

QVariant Controller::promotionPieces() const {
    if (!p->promotion.has_value()) return {};

    auto sq = (*p->promotion).to();
    auto color = sq.rank() >= 4 ?
                 disboard::Color::White : disboard::Color::Black;
    auto queen = disboard::Piece(
            color,
            disboard::Role::Queen
    );
    auto knight = disboard::Piece(
            color,
            disboard::Role::Knight
    );
    auto rook = disboard::Piece(
            color,
            disboard::Role::Rook
    );
    auto bishop = disboard::Piece(
            color,
            disboard::Role::Bishop
    );

    auto vec = QVector<disboard::Piece>{queen, knight, rook, bishop};

    return QVariant::fromValue(vec);
}

QVariant Controller::phantom() const {
    if (!p->dragged.has_value()) return {};

    auto piece = p->dragged->piece;
    return QVariant::fromValue(piece);
}

QPointF Controller::dragPos() const {
    return mDragPos;
}

void Controller::setDragPos(QPointF newValue) {
    if (mDragPos == newValue) return;
    mDragPos = newValue;
    emit dragPosChanged();
}

disboard::Square Controller::dragSq() const {
    return {
            coord_to_square(
                    mDragPos.x(), mDragPos.y(),
                    std::max(pieceSize(), 1)
            )
    };
}

QVariant Controller::highlightedSq() const {
    if (auto val = p->highlightedSq) {
        return QVariant::fromValue(*val);
    }
    return {};
}

QVariant Controller::lastSrcSq() const {
    if (auto val = p->board.lastMove(curNode())) {
        return QVariant::fromValue(
                (*val).from()
        );
    }
    return {};
}

QVariant Controller::lastDestSq() const {
    if (auto val = p->board.lastMove(curNode())) {
        return QVariant::fromValue(
                (*val).to()
        );
    }
    return {};
}

QVector<disboard::Square> Controller::hintSq() const {
    if (auto _highlightSq = p->highlightedSq) {
        const auto [hints, _] = p->board.hints(curNode(), *_highlightSq);
        return hints;
    }
    return {};
}

QVector<disboard::Square> Controller::captureSq() const {
    if (auto _highlightSq = p->highlightedSq) {
        const auto [_, captures] = p->board.hints(curNode(), *_highlightSq);
        return captures;
    }
    return {};
}

QString Controller::pgn() const {
    return p->board.pgn();
}

const disboard::Disboard& Controller::board() const {
    return p->board;
}

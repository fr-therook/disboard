#include "move.h"

using namespace disboard;

Move::Move(const disboard::Move &rhs) : impl(rhs.impl->clone()) {}

Move &Move::operator=(const disboard::Move &rhs) {
    impl = rhs.impl->clone();
    return *this;
}

Square Move::from() const { return Square{impl->from()}; }
Square Move::to() const { return Square{impl->to()}; }

bool Move::isPromotion() const { return impl->is_promotion(); }
void Move::setPromotion(Role role) { impl->set_promotion(role); }

bool Move::isEnPassant() const { return impl->is_en_passant(); }

bool Move::isCastle() const { return impl->is_castle(); }

Square Move::castleRookFrom() const {
    return Square{impl->castle_rook_from()};
}

Square Move::castleRookTo() const {
    return Square{impl->castle_rook_to()};
}

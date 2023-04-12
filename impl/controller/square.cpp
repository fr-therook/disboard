#include "square.h"

using namespace disboard;

Square::Square()
        : impl(librustdisboard::square_default()) {}

Square::Square(uint8_t file, uint8_t rank)
        : impl(librustdisboard::square_from_coords(file, rank)) {}

uint8_t Square::index() const { return impl.index; }

uint8_t Square::file() const { return impl.file(); }

uint8_t Square::rank() const { return impl.rank(); }

bool Square::operator==(Square &rhs) { return impl == rhs.impl; }

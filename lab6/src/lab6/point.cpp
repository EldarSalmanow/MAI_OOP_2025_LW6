#include <lab6/point.h>


Point::Point(std::uint64_t x,
             std::uint64_t y)
        : _x(x),
          _y(y) {}

auto Point::GetX() const -> std::uint64_t {
    return _x;
}

auto Point::GetY() const -> std::uint64_t {
    return _y;
}

#ifndef MAI_OOP_2025_POINT_H
#define MAI_OOP_2025_POINT_H

#include <cstdint>


class Point {
public:

    Point(std::uint64_t x,
          std::uint64_t y);

public:

    auto GetX() const -> std::uint64_t;

    auto GetY() const -> std::uint64_t;

private:

    std::uint64_t _x, _y;
};

#endif //MAI_OOP_2025_POINT_H
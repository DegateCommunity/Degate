/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Core/Primitive/Line.h"
#include "Core/Primitive/Circle.h"
#include "Core/Primitive/Point.h"
#include "Core/Primitive/Rectangle.h"

#include <catch.hpp>

using namespace degate;

TEST_CASE("Test in shape rectangle", "[Shape]")
{
    BoundingBox b(10, 90, 10, 90);
    Rectangle r(10, 90, 10, 90);
    r.get_bounding_box().print();
    b.print();
    REQUIRE(r.get_bounding_box() == b);

    REQUIRE(r.in_shape(50, 50) == true);
    REQUIRE(r.in_shape(10, 10) == true);
    REQUIRE(r.in_shape(90, 90) == true);

    REQUIRE(r.in_shape(50, 9) == false);

    r.shift_x(10);
    r.shift_y(10);
    REQUIRE(r.in_shape(10, 10) == false);
    REQUIRE(r.in_shape(100, 100) == true);
}

TEST_CASE("Test in shape circle", "[Shape]")
{
    Circle c(10, 10, 5);

    REQUIRE(c.in_shape(8, 8) == true);
    REQUIRE(c.in_shape(100, 100) == false);

    c.shift_x(10);
    c.shift_y(10);
    REQUIRE(c.in_shape(18, 18) == true);
}

TEST_CASE("Test in shape line", "[Shape]")
{
    const unsigned int diameter = 2;

    Line l(10, 10, 90, 90, diameter);
    REQUIRE(l.is_vertical() == false);
    REQUIRE(l.is_horizontal() == false);

    REQUIRE(l.in_shape(10, 10) == true);
    REQUIRE(l.in_shape(7, 7) == false);
    REQUIRE(l.in_shape(91, 91) == false);

    l.shift_x(10);
    l.shift_y(10);
    REQUIRE(l.in_shape(0, 0) == false);
    REQUIRE(l.in_shape(10, 10) == false);
    REQUIRE(l.in_shape(100, 100) == true);

    Line h(10, 10, 100, 10, diameter);
    REQUIRE(h.is_horizontal());

    BoundingBox b = h.get_bounding_box();
    REQUIRE(b.get_height() == diameter);
    REQUIRE(b.get_width() == 90);

    REQUIRE(h.in_shape(50, 10) == true);
    REQUIRE(h.in_shape(50, 11) == true);
    REQUIRE(h.in_shape(50, 8) == false);
    REQUIRE(h.in_shape(5, 10) == false);

    Line v(10, 10, 10, 100, diameter);
    b = v.get_bounding_box();
    REQUIRE(b.get_height() == 90);
    REQUIRE(b.get_width() == diameter);

    REQUIRE(v.is_vertical());
    REQUIRE(v.in_shape(10, 10) == true);
    REQUIRE(v.in_shape(10, 50) == true);
    REQUIRE(v.in_shape(10, 100) == true);

    REQUIRE(v.in_shape(10, 5) == false);
    REQUIRE(v.in_shape(10, 110) == false);

    REQUIRE(v.in_shape(50, 50) == false);
}

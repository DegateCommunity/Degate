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

#ifndef __POINT_H__
#define __POINT_H__

#include <string>

namespace degate
{
    class Point
    {
    private:
        float x, y;

    public:

        Point();
        Point(float x, float y);

        bool operator==(const Point& other) const;
        bool operator!=(const Point& other) const;

        float get_x() const;
        float get_y() const;

        void set_x(float x);
        void set_y(float y);

        void shift_x(float delta_x);
        void shift_y(float delta_y);

        /**
         * Calculate the distance to another point.
         */
        float get_distance(Point const& p) const;


        std::string to_string() const;
    };

    typedef std::shared_ptr<Point> Point_shptr;
}

#endif

/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2008, 2009, 2010 by Martin Schobert

  Degate is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  any later version.

  Degate is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with degate. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __DISTANCEFIELDGENERATOR_H__
#define __DISTANCEFIELDGENERATOR_H__

#include <QtGui/QImage>
#include <QColor>

namespace degate
{
    class DistanceFieldGenerator
    {
    public:
        /**
         * Prepare parameters to generate independent signed distance field images.
         *
         * @param spread : The maximum distance (in pixels) that will be scanned to find edges.
         * @param scale_factor : The scale factor to downscale the output image of the generate_distance_field function.
         * @param color : The output color of the output image.
         */
        DistanceFieldGenerator(float spread, unsigned int scale_factor, unsigned int color = qRgba(255, 255, 255, 255));

        /**
         * Generate an image with signed distance field from an input image.
         *
         * @param input_image : The input image.
         *
         * @return Return the signed distance field image from the input image (with a scale_factor downscale).
         */
        QImage generate_distance_field(const QImage& input_image) const;

    private:
        unsigned int color = qRgba(255, 255, 255, 255);
        float spread = 1;
        unsigned int scale_factor = 1;
    };
}

#endif //__DISTANCEFIELDGENERATOR_H__

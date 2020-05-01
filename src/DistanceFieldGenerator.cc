/*******************************************************************************
 * This file is inspired by the libgdx Java implementation in the Hiero software.
 *
 * Copyright 2011 Badlogic
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#include "DistanceFieldGenerator.h"

#include <cmath>
#include <QtConcurrent/QtConcurrent>
#include <boost/range/counting_range.hpp>

#define SQUARE_DISTANCE(x1, y1, x2, y2) ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))

namespace degate
{

    DistanceFieldGenerator::DistanceFieldGenerator(float spread, unsigned int scale_factor, unsigned int color) : spread(spread), scale_factor(scale_factor), color(color)
    {

    }

    QImage DistanceFieldGenerator::generate_distance_field(const QImage& input_image) const
    {
        int input_width = input_image.width();
        int input_height = input_image.height();
        int output_width = input_width / static_cast<int>(scale_factor);
        int output_height = input_height / static_cast<int>(scale_factor);
        QImage outImage(output_width, output_height, QImage::Format_ARGB32);

        // Create the input matrix.
        bool** input = new bool*[input_width];
        for(unsigned int i = 0; i < input_width; i++)
            input[i] = new bool[input_height];

        // Fill the bool matrix to know if a pixel is considered inside or outside.
        std::function<void(const unsigned int& y)> input_function = [&input_image, &input, &input_width](const unsigned int& y)
        {
            auto pixels = (QRgb*)(input_image.scanLine(y));
            for (unsigned int x = 0; x < input_width; x++)
            {
                input[x][y] = (pixels[x] & 0x808080) != 0 && (pixels[x] & 0x80000000) != 0;
            }
        };

        // Multi threaded process
        const auto& input_it = boost::counting_range<unsigned int>(0, input_height);
        QtConcurrent::blockingMap(input_it, input_function);

        // Create the output.
        std::function<void(const unsigned int& y)> output_function = [this, &output_width, &outImage, &input, &input_width, &input_height](const unsigned int& y)
        {
            auto pixels = (QRgb*)(outImage.scanLine(y));
            for (unsigned int x = 0; x < output_width; x++)
            {
                int center_x = (x * scale_factor) + (scale_factor / 2);
                int center_y = (y * scale_factor) + (scale_factor / 2);


                // Signed distance.

                bool base = input[center_x][center_y];

                int delta = std::ceil(spread);
                int start_x = std::max<int>(0, center_x - delta);
                int end_x  = std::min<int>(input_width - 1, center_x + delta);
                int start_y = std::max<int>(0, center_y - delta);
                int end_y = std::min<int>(input_height - 1, center_y + delta);

                int closest_square_distance = delta * delta;

                for (int x = start_x; x <= end_x; ++x)
                {
                    for (int y = start_y; y <= end_y; ++y)
                    {
                        if (base != input[x][y])
                        {
                            int distance = SQUARE_DISTANCE(center_x, center_y, x, y);
                            if (distance < closest_square_distance)
                            {
                                closest_square_distance = distance;
                            }
                        }
                    }
                }

                float signed_distance = (base ? 1 : -1) * std::min<float>(std::sqrt(closest_square_distance), spread);


                // Distance to RGB.

                float alpha = 0.5f + 0.5f * (signed_distance / spread);
                alpha = std::min<float>(1.0, std::max<float>(0.0, alpha));

                auto alphaByte = static_cast<unsigned int>(alpha * 0xFF);

                pixels[x] = (alphaByte << 24) | (color & 0xFFFFFF);
            }
        };

        // Multi threaded process
        const auto& output_it = boost::counting_range<unsigned int>(0, output_height);
        QtConcurrent::blockingMap(output_it, output_function);

        // Delete the input matrix.
        for(unsigned int i = 0; i < input_width; i++)
            delete[] input[i];
        delete[] input;

        return outImage;
    }
}
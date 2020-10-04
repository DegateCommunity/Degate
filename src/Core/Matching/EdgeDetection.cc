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

#include "Core/Matching/EdgeDetection.h"
#include "Core/Image/Processor/IPMedianFilter.h"

using namespace degate;

EdgeDetection::EdgeDetection(unsigned int min_x, unsigned int max_x,
                             unsigned int min_y, unsigned int max_y,
                             unsigned int median_filter_width,
                             unsigned int blur_kernel_size,
                             double sigma) :
    min_x(min_x),
    max_x(max_x),
    min_y(min_y),
    max_y(max_y),
    median_filter_width(median_filter_width),
    blur_kernel_size(blur_kernel_size),
    border(blur_kernel_size >> 1),
    sigma(sigma),
    has_path(false)
{
    setup_pipe();
}


EdgeDetection::~EdgeDetection()
{
}

unsigned int EdgeDetection::get_border() const
{
    return border;
}


void EdgeDetection::setup_pipe()
{
    debug(TM, "will extract background image (%d, %d) (%d, %d)", min_x, min_y, max_x, max_y);
    std::shared_ptr<IPCopy<TileImage_RGBA, TileImage_GS_DOUBLE>> copy_rgba_to_gs
        (new IPCopy<TileImage_RGBA, TileImage_GS_DOUBLE>(min_x, max_x, min_y, max_y));

    pipe.add(copy_rgba_to_gs);

    if (median_filter_width > 0)
    {
        std::shared_ptr<IPMedianFilter<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>> median_filter
            (new IPMedianFilter<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>(median_filter_width));

        pipe.add(median_filter);
    }


    std::shared_ptr<IPNormalize<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>> normalizer
        (new IPNormalize<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>(0, 1));
    pipe.add(normalizer);


    if (blur_kernel_size > 0)
    {
        std::shared_ptr<GaussianBlur> gaussian_b(new GaussianBlur(blur_kernel_size, blur_kernel_size, sigma));

        gaussian_b->print();
        std::shared_ptr<IPConvolve<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>> gaussian_blur
            (new IPConvolve<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>(gaussian_b));

        pipe.add(gaussian_blur);
    }
}


unsigned int EdgeDetection::get_width() const
{
    return max_x - min_x;
}

unsigned int EdgeDetection::get_height() const
{
    return max_y - min_y;
}

void EdgeDetection::set_directory(std::string const& path)
{
    directory = path;
    has_path = true;
}

std::string EdgeDetection::get_directory() const
{
    return directory;
}

bool EdgeDetection::has_directory() const
{
    return has_path;
}

void EdgeDetection::run_edge_detection(ImageBase_shptr in)
{
    ImageBase_shptr out = pipe.run(in);
    assert(out != nullptr);

    std::shared_ptr<SobelYOperator> sobel_y(new SobelYOperator());
    std::shared_ptr<IPConvolve<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>> edge_filter_x
        (new IPConvolve<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>(sobel_y));

    std::shared_ptr<SobelXOperator> sobel_x(new SobelXOperator());
    std::shared_ptr<IPConvolve<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>> edge_filter_y
        (new IPConvolve<TileImage_GS_DOUBLE, TileImage_GS_DOUBLE>(sobel_x));

    i1 = std::dynamic_pointer_cast<TileImage_GS_DOUBLE>(edge_filter_x->run(out));
    i2 = std::dynamic_pointer_cast<TileImage_GS_DOUBLE>(edge_filter_y->run(out));
    assert(i1 != nullptr && i2 != nullptr);

    if (has_path) save_normalized_image<TileImage_GS_DOUBLE>(join_pathes(directory, "01_sobelx.tif"), i1);
    if (has_path) save_normalized_image<TileImage_GS_DOUBLE>(join_pathes(directory, "02_sobely.tif"), i2);
}

TileImage_GS_DOUBLE_shptr EdgeDetection::get_horizontal_edges()
{
    return i1;
}

TileImage_GS_DOUBLE_shptr EdgeDetection::get_vertical_edges()
{
    return i2;
}


TileImage_GS_DOUBLE_shptr EdgeDetection::get_edge_magnitude_image(TileImage_GS_DOUBLE_shptr probability_map)
{
    if (i1 == nullptr || i2 == nullptr) return TileImage_GS_DOUBLE_shptr();

    TileImage_GS_DOUBLE_shptr edge_mag_image(new TileImage_GS_DOUBLE(get_width(), get_height()));

    for (unsigned int y = border + 2; y < get_height() - border - 1; y++)
    {
        for (unsigned int x = border + 2; x < get_width() - border - 1; x++)
        {
            double gx = i1->get_pixel(x, y);
            double gy = i2->get_pixel(x, y);
            double g = sqrt(pow(gx, 2) + pow(gy, 2));
            if (probability_map != nullptr)
            {
                if (probability_map->get_pixel(x, y) > 0)
                    edge_mag_image->set_pixel(x, y, g);
            }
            else
                edge_mag_image->set_pixel(x, y, g);
        }
    }

    if (has_path)
        save_normalized_image<TileImage_GS_DOUBLE>(join_pathes(directory,
                                                               "03_edge_mag_image.tif"),
                                                   edge_mag_image);
    return edge_mag_image;
}


TileImage_GS_DOUBLE_shptr EdgeDetection::get_edge_image(TileImage_GS_DOUBLE_shptr probability_map)
{
    if (i1 == nullptr || i2 == nullptr) return TileImage_GS_DOUBLE_shptr();

    TileImage_GS_DOUBLE_shptr edge_image(new TileImage_GS_DOUBLE(get_width(), get_height()));

    for (unsigned int y = border + 2; y < get_height() - border - 1; y++)
    {
        for (unsigned int x = border + 2; x < get_width() - border - 1; x++)
        {
            double gx = i1->get_pixel(x, y);
            double gy = i2->get_pixel(x, y);

            if (probability_map != nullptr)
            {
                if (probability_map->get_pixel(x, y) > 0)
                    edge_image->set_pixel(x, y, gx + gy);
            }
            else
                edge_image->set_pixel(x, y, gx + gy);
        }
    }
    if (has_path)
        save_normalized_image<TileImage_GS_DOUBLE>(join_pathes(directory,
                                                               "03_edge_image.tif"),
                                                   edge_image);

    return edge_image;
}

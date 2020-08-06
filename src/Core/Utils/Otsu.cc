/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

 Copyright 2013 by Taekgwan Kim 

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

#include <Core/Utils/Otsu.h>

using namespace degate;

Otsu::Otsu() :
	otsu_threshold(0)
{
}

Otsu::~Otsu()
{
}

double Otsu::get_otsu_threshold()
{
	return otsu_threshold;
}

void Otsu::run(TileImage_GS_DOUBLE_shptr gray)
{
	unsigned long hist_data[256] = {0,};

	for (unsigned int y = 0; y < gray->get_height(); y++)
		for (unsigned int x = 0; x < gray->get_width(); x++)
		{
			gs_byte_pixel_t h = gray->get_pixel_as<gs_byte_pixel_t>(x, y);
			assert((h >= 0) && (h < 256));
			hist_data[h]++;
		}

	unsigned long total = gray->get_height() * gray->get_width();

	double sum = 0;
	for (int t = 0; t < 256; t++)
		sum += t * hist_data[t];

	double        sum_b = 0;
	unsigned long w_b   = 0;
	unsigned long w_f   = 0;

	double var_max = 0;

	for (int t = 0; t < 256; t++)
	{
        w_b += hist_data[t];
		if (w_b == 0)
			continue;

        w_f = total - w_b;
		if (w_f == 0)
			break;
        sum_b += static_cast<double>(t * hist_data[t]);

		double m_b         = sum_b / w_b;
		double m_f         = (sum - sum_b) / w_f;
		double var_between = static_cast<double>(w_b) * static_cast<double>(w_f) * (m_b - m_f) * (m_b - m_f);

		if (var_between > var_max)
		{
            var_max        = var_between;
			otsu_threshold = t;
		}
	}
}

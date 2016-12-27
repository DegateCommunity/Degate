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

#include <Otsu.h>

using namespace degate;

Otsu::Otsu() : 
  otsu_threshold(0) {}

Otsu::~Otsu() {}

double Otsu::get_otsu_threshold() {
  return otsu_threshold;
}

void Otsu::run(TileImage_GS_DOUBLE_shptr gray) {
  unsigned long histData[256] = {0,};

  for(unsigned int y = 0; y < gray->get_height(); y++)
    for(unsigned int x = 0; x < gray->get_width(); x++) {
      gs_byte_pixel_t h = gray->get_pixel_as<gs_byte_pixel_t>(x, y);
      assert((h >= 0) && (h < 256));
      histData[h]++;
    }

  unsigned long total = gray->get_height() * gray->get_width();

  double sum = 0;
  for(int t=0; t < 256; t++)
    sum += t * histData[t];

  double sumB = 0;
  unsigned long wB = 0;
  unsigned long wF = 0;

  double varMax = 0;

  for(int t=0; t<256; t++)
  {
    wB += histData[t];
    if(wB == 0)
      continue;

    wF = total - wB;
    if(wF == 0)
      break;
    sumB += (double)(t * histData[t]);

    double mB = sumB / wB;
    double mF = (sum - sumB) /wF;
    double varBetween = (double)wB * (double)wF * (mB-mF) * (mB-mF);

    if(varBetween > varMax)
    {
      varMax = varBetween;
      otsu_threshold = t;
    }
  }
}

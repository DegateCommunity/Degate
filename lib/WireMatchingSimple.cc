/* -*-c++-*-
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
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

#include <WireMatchingSimple.h>
#include <IPPipe.h>
#include <IPCopy.h>
#include <IPConvolve.h>
#include <IPNormalize.h>
#include <IPThresholding.h>
#include <ImageManipulation.h>
#include <IPImageWriter.h>
#include <FilterKernel.h>

using namespace degate;

void WireMatchingSimple::setup_pipe(unsigned int min_x, unsigned int max_x, 
				    unsigned int min_y, unsigned int max_y,
				    bool match_horizontal_lines,
				    bool match_vertical_lines) {

  std::tr1::shared_ptr<IPCopy<TempImage_RGBA, TempImage_GS_DOUBLE> > copy_rgba_to_gs
    (new IPCopy<TempImage_RGBA, TempImage_GS_DOUBLE>(min_x, max_x, min_y, max_y) );

  std::tr1::shared_ptr<IPImageWriter<TempImage_GS_DOUBLE> > writer1
    (new IPImageWriter<TempImage_GS_DOUBLE>("/tmp/1.tif") );

  std::tr1::shared_ptr<GaussianBlur> GaussianB(new GaussianBlur(11, 11));
  GaussianB->print();
  std::tr1::shared_ptr<IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE> > gaussian_blur
    (new IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(GaussianB) );

  std::tr1::shared_ptr<IPImageWriter<TempImage_GS_DOUBLE> > writer2
    (new IPImageWriter<TempImage_GS_DOUBLE>("/tmp/2.tif") );

  std::tr1::shared_ptr<FilterKernel> sobel;
  if(match_horizontal_lines && match_vertical_lines) 
    sobel = SobelOperator_shptr(new SobelOperator());
  else if(match_horizontal_lines) sobel = SobelYOperator_shptr(new SobelYOperator());
  else if(match_vertical_lines) sobel = SobelYOperator_shptr(new SobelYOperator());

  std::tr1::shared_ptr<IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE> > edge_filter
    (new IPConvolve<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(sobel) );

  std::tr1::shared_ptr<IPImageWriter<TempImage_GS_DOUBLE> > writer3
    (new IPImageWriter<TempImage_GS_DOUBLE>("/tmp/3.tif") );

  std::tr1::shared_ptr<IPNormalize<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE> > normalize
    (new IPNormalize<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(0, 1) );

  std::tr1::shared_ptr<IPImageWriter<TempImage_GS_DOUBLE> > writer4
    (new IPImageWriter<TempImage_GS_DOUBLE>("/tmp/4.tif") );

  /*
  std::tr1::shared_ptr<IPThresholding<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE> > thresholding
    (new IPThresholding<TempImage_GS_DOUBLE, TempImage_GS_DOUBLE>(0.55) );

  std::tr1::shared_ptr<IPImageWriter<TempImage_GS_DOUBLE> > writer5
    (new IPImageWriter<TempImage_GS_DOUBLE>("/tmp/5.tif") );
  */
  pipe.add(copy_rgba_to_gs);
  pipe.add(writer1);
  //pipe.add(gaussian_blur);
  pipe.add(writer2);
  pipe.add(edge_filter);
  pipe.add(writer3);
  pipe.add(normalize);
  pipe.add(writer4);
  //pipe.add(thresholding);
  //pipe.add(writer5);

}

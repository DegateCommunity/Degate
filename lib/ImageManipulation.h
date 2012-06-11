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

#ifndef __IMAGEMANIPULATION_H__
#define __IMAGEMANIPULATION_H__

#include <BoundingBox.h>
#include <Image.h>
#include <FilterKernel.h>
#include <Statistics.h>
#include <ImageStatistics.h>

#include <boost/format.hpp>

namespace degate {

  /**
   * Flip image in place from left to right.
   */
  template<typename ImageType>
  void flip_left_right(std::shared_ptr<ImageType> img) {
    if(img->get_width() == 1) return;

    for(unsigned int y = 0; y < img->get_height(); y++)
      for(unsigned int x = 0; x < (img->get_width() >> 1); x++) {
	unsigned int other_x = img->get_width() - 1 - x;
	typename ImageType::pixel_type p1 = img->get_pixel(x, y);
	typename ImageType::pixel_type p2 = img->get_pixel(other_x, y);
	img->set_pixel(x, y, p2);
	img->set_pixel(other_x, y, p1);
      }
  }

  /**
   * Flip image in place from top to down.
   */
  template<typename ImageType>
  void flip_up_down(std::shared_ptr<ImageType> img) {
    if(img->get_height() == 1) return;

    for(unsigned int y = 0; y < (img->get_height() >> 1); y++)
      for(unsigned int x = 0; x < img->get_width(); x++) {
	unsigned int other_y = img->get_height() - 1 - y;
	typename ImageType::pixel_type p1 = img->get_pixel(x, y);
	typename ImageType::pixel_type p2 = img->get_pixel(x, other_y);
	img->set_pixel(x, y, p2);
	img->set_pixel(x, other_y, p1);
      }
  }

  /**
   * Flip image in place from top to down and from left to right.
   */
  template<typename ImageType>
  void flip_both(std::shared_ptr<ImageType> img) {
    flip_up_down<ImageType>(img);
    flip_left_right<ImageType>(img);
  }

  /**
   * Convert an RGBA pixel to a hue value.
   */
   inline double rgba_to_hue(rgba_pixel_t p) {
     int red = MASK_R(p);
     int green = MASK_G(p);
     int blue = MASK_B(p);

     int max = std::max(red, std::max(green, blue));
     int min = std::min(red, std::min(green, blue));
     double delta = max - min;
     double h = 0;

     if(max == min) h = 0;
     else if(max == red) h = 60.0 *  (double)(green-blue)/delta;
     else if(max == green) h = 60.0 * (2.0 +(double)(blue-red)/delta);
     else if(max == blue) h = 60.0 * (4.0 + (double)(red-green)/delta);
     if(h < 0) h+=360;

     //h *= 255.0/360.0;
     return h;

   }

  /**
   * Convert an RGBA pixel to a saturation value.
   */

   inline double rgba_to_saturation(rgba_pixel_t p) {
     int red = MASK_R(p);
     int green = MASK_G(p);
     int blue = MASK_B(p);

     double max = std::max(red, std::max(green, blue));
     double min = std::min(red, std::min(green, blue));

     if(max == 0) return 0;
     else return (max - min) / max;
   }

  /**
   * Convert an RGBA pixel to a lightness value.
   */

   inline double rgba_to_lightness(rgba_pixel_t p) {
     int red = MASK_R(p);
     int green = MASK_G(p);
     int blue = MASK_B(p);

     return std::max(red, std::max(green, blue));
   }


  /**
   * Convert a pixel from a source type to a destination type. The
   * default implementation will just make a copy of the pixel.
   */
  template<typename PixelTypeDst, typename PixelTypeSrc>
  inline PixelTypeDst convert_pixel(PixelTypeSrc p) {
    return p;
  }


  /**
   * Convert pixel value from from rgba -> byte.
   */
  template<>
  inline gs_byte_pixel_t convert_pixel<gs_byte_pixel_t, rgba_pixel_t>(rgba_pixel_t p) {
    return RGBA_TO_GS_BY_VAL(p);
  }

  /**
   * Convert pixel value from from rgba -> double.
   */
  template<>
  inline gs_double_pixel_t convert_pixel<gs_double_pixel_t, rgba_pixel_t>(rgba_pixel_t p) {
    return RGBA_TO_GS_BY_VAL(p);
  }



  /**
   * Convert pixel value from from byte -> rgba.
   */
  template<>
  inline rgba_pixel_t convert_pixel<rgba_pixel_t, gs_byte_pixel_t>(gs_byte_pixel_t p) {
    return MERGE_CHANNELS(p, p, p, 255);
  }

  /**
   * Convert pixel value from from double -> rgba.
   */
  template<>
  inline rgba_pixel_t convert_pixel<rgba_pixel_t, gs_double_pixel_t>(gs_double_pixel_t p) {
    gs_byte_pixel_t b = p;
    return MERGE_CHANNELS(b, b, b, 255);
  }





  /*
   * Get pixel value as ...
   */
  template<typename PixelTypeDst, typename ImageTypeSrc>
  inline PixelTypeDst get_pixel_as(typename std::shared_ptr<ImageTypeSrc> img,
				   unsigned int x, unsigned int y) {
    return convert_pixel<PixelTypeDst, typename ImageTypeSrc::pixel_type>(img->get_pixel(x, y));
  }

  /*
   * Set a pixel value as ...
   */
  template<typename PixelTypeSrc, typename ImageTypeDst>
  inline void set_pixel_as(typename std::shared_ptr<ImageTypeDst> img,
		    unsigned int x, unsigned int y, PixelTypeSrc p) {

    img->get_pixel(x, y, convert_pixel<typename ImageTypeDst::pixel_type, PixelTypeSrc>(p));
  }


  /**
   * Copy an image.
   * Copy the source image into the destination image. If the images differ in
   * pixel types, the pixel values will be converted. The images can also differ
   * in size. Only the region is copied, which is present in both images. It is
   * possible that the image \p dst is not completely overwritten, if the image
   * \p src is not large enough.
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void copy_image(std::shared_ptr<ImageTypeDst> dst,
		  std::shared_ptr<ImageTypeSrc> src) {


    unsigned int h = std::min(src->get_height(), dst->get_height());
    unsigned int w = std::min(src->get_width(), dst->get_width());

    for(unsigned int y = 0; y < h; y++)
      for(unsigned int x = 0; x < w; x++)
	dst->set_pixel(x, y, src->get_pixel_as<typename ImageTypeDst::pixel_type>(x, y));
  }


  /**
   * Extract a partial image from \p src with the region defined by the parameters
   * into a destination image \p dst. Clipping occurs, if the destination image is
   * smaller than the region or the image \p src.
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void extract_partial_image(std::shared_ptr<ImageTypeDst> dst,
			     std::shared_ptr<ImageTypeSrc> src,
			     unsigned int min_x, unsigned int max_x,
			     unsigned int min_y, unsigned int max_y) {

    assert(min_x < max_x);
    assert(min_y < max_y);

    unsigned int h = std::min(std::min(std::min(src->get_height(), max_y), dst->get_height()), max_y - min_y);
    unsigned int w = std::min(std::min(std::min(src->get_width(), max_x), dst->get_width()), max_x - min_x);

    unsigned int dst_x = 0, dst_y = 0, x, y;

    for(y = min_y; y < min_y + h; y++, dst_y++) {
      for(x = min_x, dst_x = 0; x < min_x + w; x++, dst_x++)
	dst->set_pixel(dst_x, dst_y,
		       src->get_pixel_as<typename ImageTypeDst::pixel_type>(x, y));
    }
  }

  /**
   * Extract a partial image.
   * @see extract_partial_image()
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void extract_partial_image(std::shared_ptr<ImageTypeDst> dst,
			     std::shared_ptr<ImageTypeSrc> src,
			     BoundingBox const& bounding_box) {

    extract_partial_image<ImageTypeDst, ImageTypeSrc>(dst, src,
						      bounding_box.get_min_x(),
						      bounding_box.get_max_x(),
						      bounding_box.get_min_y(),
						      bounding_box.get_max_y());
  }


  /**
   * Convert an image to greyscale.
   * You can get the same effect if you copy_image() an RGBA into a greyscale
   * image and implicitly use the auto conversion. This function is useful, if
   * you don't have two different image types. Then there would be no auto
   * conversion.
   * You can use that function with \p dst = \p src . Then the image is converted
   * in place.
   * Like copy_image() this function works only on the region, in which both images
   * intersect.
   * @see copy_image()
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void convert_to_greyscale(std::shared_ptr<ImageTypeDst> dst,
			    std::shared_ptr<ImageTypeSrc> src) {

    unsigned int h = std::min(src->get_height(), dst->get_height());
    unsigned int w = std::min(src->get_width(), dst->get_width());

    for(unsigned int y = 0; y < h; y++)
      for(unsigned int x = 0; x < w; x++) {
	gs_byte_pixel_t p = src->get_pixel_as<gs_byte_pixel_t>(x, y);
	dst->set_pixel_as<gs_byte_pixel_t>(x, y, p);
      }
  }

  /**
   * In place conversion to a greyscale image.
   * @see convert_to_greyscale(std::shared_ptr<ImageTypeDst>, std::shared_ptr<ImageTypeSrc>)
   */

  template<typename ImageType>
  void convert_to_greyscale(std::shared_ptr<ImageType> img) {
    convert_to_greyscale<ImageType, ImageType>(img, img);
  }


  /**
   * Scale a source image down by factor 2.
   * You can scale images in place.
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void scale_down_by_2(std::shared_ptr<ImageTypeDst> dst,
		       std::shared_ptr<ImageTypeSrc> src) {


    unsigned int dst_x, dst_y, src_x, src_y;

    for(dst_y = 0; dst_y < dst->get_height(); dst_y++) {

      src_y = dst_y * 2;

      for(dst_x = 0; dst_x < dst->get_width(); dst_x++) {

	src_x = dst_x * 2;

	// 1 2
	// 3 4

	int i = 1;
	unsigned int r = 0, g = 0, b = 0, a = 0;

	rgba_pixel_t pix = src->get_pixel_as<rgba_pixel_t>(src_x, src_y);
	r += MASK_R(pix);
	g += MASK_G(pix);
	b += MASK_B(pix);
	a += MASK_A(pix);

	if(src_x + 1 < src->get_width()) {
	  pix = src->get_pixel_as<rgba_pixel_t>(src_x + 1, src_y);
	  i++;
	  r += MASK_R(pix);
	  g += MASK_G(pix);
	  b += MASK_B(pix);
	  a += MASK_A(pix);
	}

	if(src_y + 1 < src->get_height()) {
	  pix = src->get_pixel_as<rgba_pixel_t>(src_x, src_y + 1);
	  i++;
	  r += MASK_R(pix);
	  g += MASK_G(pix);
	  b += MASK_B(pix);
	  a += MASK_A(pix);
	}

	if(src_x + 1 < src->get_width() && src_y + 1 < src->get_height()) {
	  pix = src->get_pixel_as<rgba_pixel_t>(src_x + 1, src_y + 1);
	  i++;
	  r += MASK_R(pix);
	  g += MASK_G(pix);
	  b += MASK_B(pix);
	  a += MASK_A(pix);
	}

	r /= i;
	g /= i;
	b /= i;
	a /= i;

	dst->set_pixel_as<rgba_pixel_t>(dst_x, dst_y, MERGE_CHANNELS(r, g, b, a));
      }
    }
  }


  /**
   * Scale a source image down by factor 2.
   * @exception DegateRuntimeException This excpetion is thrown if the
   *  destination image has no dimension definition.
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void scale_down_by_power_of_2(std::shared_ptr<ImageTypeDst> dst,
				std::shared_ptr<ImageTypeSrc> src) {

    if(dst->get_width() == 0) throw DegateRuntimeException("Invalid image dimension for destination image.");

    unsigned int scaling = lrint((double)src->get_width() / (double)dst->get_width());

    if(scaling == 1)
      copy_image<ImageTypeDst, ImageTypeSrc>(dst, src);
    else if(scaling == 2)
      scale_down_by_2<ImageTypeDst, ImageTypeSrc>(dst, src);
    else {
      std::shared_ptr<ImageTypeDst> tmp(new ImageTypeDst(src->get_width(), src->get_height()));
      copy_image<ImageTypeDst, ImageTypeSrc>(tmp, src);

      scaling >>= 1;
      for(unsigned int i = 0; i < scaling - 1; i*=2) {
	scale_down_by_2<ImageTypeDst, ImageTypeDst>(tmp, tmp);
      }
      scale_down_by_2<ImageTypeDst, ImageTypeDst>(dst, tmp);
    }

  }

  /**
   * Clear an image.
   */
  template<typename ImageType>
  void clear_image(std::shared_ptr<ImageType> img) {

    for(unsigned int y = 0; y < img->get_height(); y++)
      for(unsigned int x = 0; x < img->get_width(); x++)
	img->set_pixel(x, y, 0);
  }



  /**
   * Helper function to load existing images in a degate image format.
   * We assume that the file or directory, where the image is stored,
   * exists.
   * @exception InvalidPathException This exception is thrown, if
   *   the \p path doen't exists.
   */

  template<typename ImageType>
  std::shared_ptr<ImageType> load_degate_image(unsigned int width, unsigned int height,
						    std::string const& path) {
    if(!file_exists(path)) {
      boost::format fmter("Error in load_degate_image(): The image file or directory %1% does not exist.");
      fmter % path;
      throw InvalidPathException(fmter.str());
    }
    return std::shared_ptr<ImageType>(new ImageType(width, height, path));
  }



  /**
   * Normalize a single channel image.
   * Source and destination image can be the same image.
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void normalize(std::shared_ptr<ImageTypeDst> dst,
		 std::shared_ptr<ImageTypeSrc> src,
		 double lower_bound = 0, double upper_bound = 1) {

    assert_is_single_channel_image<ImageTypeSrc>();

    typename ImageTypeSrc::pixel_type src_min = get_minimum<ImageTypeSrc>(src);
    typename ImageTypeSrc::pixel_type src_max = get_maximum<ImageTypeSrc>(src);

    if(src_max - src_min == 0) return;

    double shift = -src_min;
    double factor = (double)(upper_bound - lower_bound) / (double)(src_max - src_min);

    /*
    std::cout
      << "lower bound: " << lower_bound << std::endl
      << "upper bound: " << upper_bound << std::endl
      << std::endl
      << "min val    : " << src_min << std::endl
      << "max val    : " << src_max << std::endl
      << std::endl
      << "factor     : " << factor << std::endl
      << std::endl
      ;
    */

    unsigned int h = std::min(src->get_height(), dst->get_height());
    unsigned int w = std::min(src->get_width(), dst->get_width());

    for(unsigned int y = 0; y < h; y++) {
      for(unsigned int x = 0; x < w; x++) {
	typename ImageTypeDst::pixel_type p =
	  src->get_pixel_as<typename ImageTypeDst::pixel_type>(x, y);

	double d = ((double)p + shift) * factor + lower_bound;
	if(d < lower_bound) {
	  if(abs(lower_bound - d) < 0.001)
	    d = lower_bound;
	  std::cout << "transformed value "<< p << " beyond lower bound: " << d << std::endl;
	  //d = lower_bound;
	}
	else if(d > upper_bound) {
	  if(abs(d - upper_bound) < 0.001)
	    d = upper_bound;
	  std::cout << "transformed value "<< p << " beyond upper bound: " << d << std::endl;

	}
	assert(d >= lower_bound);
	assert(d <= upper_bound);
	dst->set_pixel_as<double>(x, y, d);
      }
    }

  }


  /**
   * Normalize a single channel image in place.
   */
  template<typename ImageType>
  void normalize(std::shared_ptr<ImageType> img,
		 double lower_bound = 0, double upper_bound = 1) {
    normalize<ImageType, ImageType>(img, img, lower_bound, upper_bound);
  }

  /**
   * Thresholding a single channel image.
   * Source and destination image can be the same image.
   * The thresholding sets a pixel value to 0 if it is below the threshold or
   * to a non-0 value if it is greater or equal than the trheshold.
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void thresholding_image(std::shared_ptr<ImageTypeDst> dst,
			  std::shared_ptr<ImageTypeSrc> src,
			  double threshold) {

    assert_is_single_channel_image<ImageTypeSrc>();

    unsigned int h = std::min(src->get_height(), dst->get_height());
    unsigned int w = std::min(src->get_width(), dst->get_width());

    for(unsigned int y = 0; y < h; y++) {
      for(unsigned int x = 0; x < w; x++) {
	typename ImageTypeDst::pixel_type p =
	  src->get_pixel_as<typename ImageTypeDst::pixel_type>(x, y);
	dst->set_pixel_as<double>(x, y, p >= threshold ? 1 : 0);
      }
    }
  }

  /**
   * Convolve a single channel source image with a filter kernel
   * and write it into a destination image.
   * Depending on the filter kernel size there is a region next to the
   * image boundary that you cannot use for further processing.
   */
  template<typename ImageTypeDst, typename ImageTypeSrc>
  void convolve(std::shared_ptr<ImageTypeDst> dst,
		std::shared_ptr<ImageTypeSrc> src,
		FilterKernel_shptr kernel) {

    assert_is_single_channel_image<ImageTypeSrc>();

    clear_image<ImageTypeDst>(dst);

    unsigned int h = std::min(src->get_height(), dst->get_height());
    unsigned int w = std::min(src->get_width(), dst->get_width());

    unsigned int x, y, i, j;

    for(y = kernel->get_center_row(); y < h - kernel->get_center_row(); y++) {
      for(x = kernel->get_center_column(); x < w - kernel->get_center_column(); x++) {

	double accu = 0;

	for(i = 0; i < kernel->get_columns(); i++ ) {
	  for(j = 0; j < kernel->get_rows(); j++ ) {

	    typename ImageTypeSrc::pixel_type p =
	      src->get_pixel(x - kernel->get_center_column() + i,
			     y - kernel->get_center_row() + j);

	    double k = kernel->get(kernel->get_columns() - 1 - i,
				   kernel->get_rows() - 1 - j);
	    accu += k * p;
	  }
	}
	dst->set_pixel_as<double>(x, y, accu);
      }
    }
  }


  /**
   * Filter an (RBGA) image.
   *
   * @param threshold The threshold parameter is directly passed to the calculate()
   *   method of the calculation policy class.
   * @exception DegateRuntimeException This exception is thrown if
   *   your images are to small for the kernel or if the width of the kernel is
   *   to small.
   */

  template<typename ImageTypeDst, typename ImageTypeSrc, typename FunctionPolicy>
  void filter_image(std::shared_ptr<ImageTypeDst> dst,
		    std::shared_ptr<ImageTypeSrc> src,
		    unsigned int kernel_width = 3,
		    unsigned int threshold = 3) {

    if(kernel_width <= 1)
      throw DegateRuntimeException("Error in filter_image(). Kernel width is to small.");

    unsigned int width = std::min(src->get_width(), dst->get_width());
    unsigned int height = std::min(src->get_height(), dst->get_height());

    if(width < kernel_width || height < kernel_width)
      throw DegateRuntimeException("Error in filter_image(). One of the images is to small.");

    unsigned int kernel_center = kernel_width / 2;

    width -= (kernel_width - kernel_center);
    height -= (kernel_width - kernel_center);

    for(unsigned int y = kernel_center; y < height; y++) {
      for(unsigned x = kernel_center; x < width; x++) {

	typename ImageTypeSrc::pixel_type out =
	  FunctionPolicy::calculate(src,
				    x, y,
				    x - kernel_center,
				    x - kernel_center + kernel_width,
				    y - kernel_center,
				    y - kernel_center + kernel_width,
				    threshold);

	dst->set_pixel_as<typename ImageTypeSrc::pixel_type>(x, y, out);
      }
    }

  }


}

#endif

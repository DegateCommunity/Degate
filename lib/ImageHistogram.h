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

#ifndef __IMAGEHISTOGRAM_H__
#define __IMAGEHISTOGRAM_H__

#include <Image.h>
#include <degate_exceptions.h>
#include <TypeConstraints.h>
#include <ImageManipulation.h>

#include <fstream>
#include <iostream>
#include <boost/format.hpp>

namespace degate {

  template<typename KeyType, typename ValueType>
  class ImageHistogram {

  private:
    typedef std::map<KeyType, ValueType> map_type;
    map_type histogram;
    unsigned int counts;

    double from, to, class_width;

  protected:

    void check_bounding_box(BoundingBox const& bb, ImageBase_shptr img) const {

      if(std::max(bb.get_min_x(), bb.get_max_x()) >= (int)img->get_width() ||
	 std::max(bb.get_min_y(), bb.get_max_y()) >= (int)img->get_height() ||
	 std::min(bb.get_min_x(), bb.get_max_x()) < 0 ||
	 std::min(bb.get_min_y(), bb.get_max_y()) < 0) {
	boost::format f("Error in add_area(). Defined region %1% is beyond image boundary.");
	f % bb.to_string();
	throw DegateRuntimeException(f.str());
      }

      if(bb.get_height() == 0 && bb.get_width() == 0)
	throw DegateRuntimeException("Bounding box has zero size");
    }

    KeyType to_class(ValueType v, ValueType from, ValueType to, ValueType class_width) const {
      //unsigned int classes = (to - from) / class_width;
      //unsigned int class_num = v / class_width;
      //KeyType cl = v * class_width;

      unsigned int c = 0;
      for(ValueType i = from; i < to; i+= class_width, c++) {
	if(v >= (ValueType)c*class_width && v < (ValueType)(c+1)*class_width)
	  return (ValueType)c*class_width;
      }

    }

  public:

    ImageHistogram(double _from, double _to, double _class_width) :
      counts(0),
      from(_from),
      to(_to),
      class_width(_class_width) {}

    virtual ~ImageHistogram() {}

    virtual void add(KeyType k) {
      histogram[to_class(k, from, to, class_width)] += 1;
      counts++;
    }

    virtual ValueType get(KeyType k) const {
      if(counts == 0) return 0;

      else {
	typename map_type::const_iterator found = histogram.find(to_class(k, from, to, class_width));
	if(found == histogram.end()) return 0;
	else return (*found).second / (double)counts;
      }
    }

    virtual ValueType get_for_rgb(rgba_pixel_t) const = 0;

    virtual void save_histogram(std::string const& path) const {

      std::ofstream histogram_file;
      histogram_file.open(path.c_str());

      if(counts > 0)
	for(typename map_type::const_iterator iter = histogram.begin(); iter != histogram.end(); ++iter) {
	  double frequency = (*iter).second / (double)counts;
	  histogram_file << (*iter).first << " " << frequency << std::endl;
	}

      histogram_file.close();
    }


  };



  class HueImageHistogram : public ImageHistogram<double, double> {

  public:

    HueImageHistogram() : ImageHistogram<double, double>(0, 360, 1) {}

    template<class ImageType>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {

      assert_is_multi_channel_image<ImageType>();
      check_bounding_box(bb, img);

      for(unsigned int y = (unsigned int)bb.get_min_y(); y <= (unsigned int)bb.get_max_y(); y++)
	for(unsigned int x = (unsigned int)bb.get_min_x(); x <= (unsigned int)bb.get_max_x(); x++)
	  add(rgba_to_hue(img->get_pixel(x, y)));
    }


    virtual double get_for_rgb(rgba_pixel_t pixel) const {
      return get(rgba_to_hue(pixel));
    }

  };



  class SaturationImageHistogram : public ImageHistogram<double, double> {

  public:

    SaturationImageHistogram() : ImageHistogram<double, double>(0, 1, 0.01) {}

    template<class ImageType>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {

      assert_is_multi_channel_image<ImageType>();
      check_bounding_box(bb, img);

      for(unsigned int y = (unsigned int)bb.get_min_y(); y <= (unsigned int)bb.get_max_y(); y++)
	for(unsigned int x = (unsigned int)bb.get_min_x(); x <= (unsigned int)bb.get_max_x(); x++)
	  add(rgba_to_saturation(img->get_pixel(x, y)));
    }

    virtual double get_for_rgb(rgba_pixel_t pixel) const {
      return get(rgba_to_saturation(pixel));
    }

  };

  class LightnessImageHistogram : public ImageHistogram<double, double> {

  public:

    LightnessImageHistogram() : ImageHistogram<double, double>(0, 255, 1) {}

    template<class ImageType>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {

      assert_is_multi_channel_image<ImageType>();
      check_bounding_box(bb, img);

      for(unsigned int y = (unsigned int)bb.get_min_y(); y <= (unsigned int)bb.get_max_y(); y++)
	for(unsigned int x = (unsigned int)bb.get_min_x(); x <= (unsigned int)bb.get_max_x(); x++)
	  add(rgba_to_lightness(img->get_pixel(x, y)));
    }

    virtual double get_for_rgb(rgba_pixel_t pixel) const {
      return get(rgba_to_lightness(pixel));
    }

  };



  class RedChannelImageHistogram : public ImageHistogram<double, double> {

  public:

    RedChannelImageHistogram() : ImageHistogram<double, double>(0, 255, 1) {}

    template<class ImageType>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {

      assert_is_multi_channel_image<ImageType>();
      check_bounding_box(bb, img);

      for(unsigned int y = (unsigned int)bb.get_min_y(); y <= (unsigned int)bb.get_max_y(); y++)
	for(unsigned int x = (unsigned int)bb.get_min_x(); x <= (unsigned int)bb.get_max_x(); x++)

	  add(MASK_R(img->get_pixel(x, y)));
    }


    virtual double get_for_rgb(rgba_pixel_t pixel) const {
      return get(MASK_R(pixel));
    }

  };



  class GreenChannelImageHistogram : public ImageHistogram<double, double> {

  public:

    GreenChannelImageHistogram() : ImageHistogram<double, double>(0, 255, 1) {}

    template<class ImageType>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {

      assert_is_multi_channel_image<ImageType>();
      check_bounding_box(bb, img);

      for(unsigned int y = (unsigned int)bb.get_min_y(); y <= (unsigned int)bb.get_max_y(); y++)
	for(unsigned int x = (unsigned int)bb.get_min_x(); x <= (unsigned int)bb.get_max_x(); x++)
	  add(MASK_G(img->get_pixel(x, y)));
    }

    virtual double get_for_rgb(rgba_pixel_t pixel) const {
      return get(MASK_G(pixel));
    }

  };



  class BlueChannelImageHistogram : public ImageHistogram<double, double> {

  public:

    BlueChannelImageHistogram() : ImageHistogram<double, double>(0, 255, 1) {}

    template<class ImageType>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {

      assert_is_multi_channel_image<ImageType>();
      check_bounding_box(bb, img);

      for(unsigned int y = (unsigned int)bb.get_min_y(); y <= (unsigned int)bb.get_max_y(); y++)
	for(unsigned int x = (unsigned int)bb.get_min_x(); x <= (unsigned int)bb.get_max_x(); x++)
	  add(MASK_B(img->get_pixel(x, y)));

    }

    virtual double get_for_rgb(rgba_pixel_t pixel) const {
      return get(MASK_B(pixel));
    }


  };


  /*
  class LocalStdDevImageHistogram : public ImageHistogram<double, double> {

  private:
    unsigned int radius;

  public:

    LocalStdDevImageHistogram(double from, double to, double class_width, unsigned int _radius = 2) :
      ImageHistogram<double, double>(from, to, class_width),
      radius(_radius) {}

    virtual ~LocalStdDevImageHistogram() {}

    template<class ImageType, class EvaluateFunc>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {

      assert_is_multi_channel_image<ImageType>();
      check_bounding_box(bb, img);

      for(unsigned int y = (unsigned int)bb.get_min_y(); y <= (unsigned int)bb.get_max_y(); y++) {
	for(unsigned int x = (unsigned int)bb.get_min_x(); x <= (unsigned int)bb.get_max_x(); x++) {

	  std::vector<double> v = fill_vector<ImageType, EvaluateFunc>(img, bb, x, y, radius);
	  add(standard_deviation<double>(v));
	}
      }
    }

  protected:

    template<class ImageType, class EvaluateFunc>
    inline std::vector<double> fill_vector(std::shared_ptr<ImageType> img, BoundingBox const& bb,
				    unsigned int x, unsigned int y, unsigned int radius) {
      std::vector<double> v;
      unsigned int min_x = bb.get_min_x() + radius < x ? x - radius : bb.get_min_x();
      unsigned int max_x = (x + radius < (unsigned int)bb.get_max_x()) ? x + radius : bb.get_max_x();
      unsigned int min_y = bb.get_min_y() + radius < y ? y - radius : bb.get_min_y();
      unsigned int max_y = (y + radius < (unsigned int)bb.get_max_y()) ? y + radius : bb.get_max_y();

      for(unsigned int _y = min_y; _y < max_y; _y++)
	for(unsigned int _x = min_x; _x < max_x; _x++)
	  v.push_back(EvaluateFunc::func(img->get_pixel(_x, _y)));
      return v;
    }


  };


  class HueStdDevImageHistogram : public LocalStdDevImageHistogram {
  private:
    struct rgba_to_hue_calculation {
      static double func(rgba_pixel_t pix) { return rgba_to_hue(pix); }
    };

  public:
    HueStdDevImageHistogram(unsigned int radius = 2) : LocalStdDevImageHistogram(0, 255, 1, radius) {}

    template<class ImageType>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {
      LocalStdDevImageHistogram::add_area<ImageType, rgba_to_hue_calculation>(img, bb);
    }
  };


  class SaturationStdDevImageHistogram : public LocalStdDevImageHistogram {
  private:
    struct rgba_to_sat_calculation {
      static double func(rgba_pixel_t pix) { return rgba_to_saturation(pix); }
    };

  public:
    SaturationStdDevImageHistogram(unsigned int radius = 2) : LocalStdDevImageHistogram(0, 1, 0.05, radius) {}

    template<class ImageType>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {
      LocalStdDevImageHistogram::add_area<ImageType, rgba_to_sat_calculation>(img, bb);
    }
  };



  class LightnessStdDevImageHistogram : public LocalStdDevImageHistogram {
  private:
    struct rgba_to_lightness_calculation {
      static double func(rgba_pixel_t pix) { return rgba_to_lightness(pix); }
    };

  public:
    LightnessStdDevImageHistogram(unsigned int radius = 2) : LocalStdDevImageHistogram(0, 255, 1, radius) {}

    template<class ImageType>
    void add_area(std::shared_ptr<ImageType> img, BoundingBox const& bb) {
      LocalStdDevImageHistogram::add_area<ImageType, rgba_to_lightness_calculation>(img, bb);
    }
  };

  */

}

#endif

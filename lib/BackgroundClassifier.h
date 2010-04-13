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

#ifndef __BACKGROUNDCLASSIFIER_H__
#define __BACKGROUNDCLASSIFIER_H__

#include <Image.h>
#include <degate_exceptions.h>
#include <TypeConstraints.h>
#include <ImageManipulation.h>
#include <ImageHistogram.h>

#include <fstream>
#include <iostream>
#include <boost/format.hpp>

#include <list>
#include <vector>
#include <adaboost.hpp>

namespace degate {

  typedef std::pair<unsigned int, unsigned int> coord_type;
  typedef std::list<coord_type> coord_list;


  class BackgroundClassifierBase : public Classifier<coord_type> {
  public:
    virtual ~BackgroundClassifierBase() {}
    virtual void add_background_areas(std::list<BoundingBox> const& bg_areas) = 0;
    virtual void add_foreground_areas(std::list<BoundingBox> const& fg_areas) = 0;
    virtual int recognize(coord_type & v) = 0;
  };


  typedef std::vector<Classifier<coord_type> *> classifier_list_type;



  template<class ImageType, typename HistogramType>
  class BackgroundClassifier : public BackgroundClassifierBase {
    
  private:
    
    const std::tr1::shared_ptr<ImageType> img;
    HistogramType hist_bg, hist_fg;
    const unsigned int width;
    const unsigned int threshold;
    const std::string cl_name;
    
  public:
    
    
    BackgroundClassifier(std::tr1::shared_ptr<ImageType> _img,
			 unsigned int _width,
			 unsigned int _threshold,
			 std::string const& _name) :
      img(_img),
      width(_width),
      threshold(_threshold),
      cl_name(_name) {
    }
    

    std::string get_name() const { 
      return cl_name;
    }


    void add_background_areas(std::list<BoundingBox> const& bg_areas) {

      for(std::list<BoundingBox>::const_iterator iter = bg_areas.begin(); 
	  iter != bg_areas.end(); ++iter) {
	hist_bg.add_area(img, *iter);
      }
    }


    void add_foreground_areas(std::list<BoundingBox> const& fg_areas) {

      
      for(std::list<BoundingBox>::const_iterator iter = fg_areas.begin(); 
	  iter != fg_areas.end(); ++iter) {
	hist_fg.add_area(img, *iter);
      }
    }


    /**
     * @todo Cache data
     */ 
    int recognize(coord_type & v) {
      unsigned int sum = 0;
      
      int radius = width >> 1;
      int x = v.first, y = v.second;

      if((x > radius && x < (int)img->get_width() - radius) &&
	 (y > radius && y < (int)img->get_height() - radius)) {
	
	for(int _y = -radius; _y < radius; _y++)
	  for(int _x = -radius; _x < radius; _x++) {
	    rgba_pixel_t p = img->get_pixel(x + _x, y + _y);
	    if(hist_fg.get_for_rgb(p) > hist_bg.get_for_rgb(p)) sum++;
	  }
      }
      
      
      if(sum >= threshold) return 1;
      else return -1;
    }
  };


  /*
  template<class ImageType>
  class GradientClassifier : public Classifier<coord_type> {

  private:
    const std::tr1::shared_ptr<ImageType> img;
    const unsigned int width;
  public:

    BackgroundClassifier(std::tr1::shared_ptr<ImageType> _img,
			 unsigned int _width) {}

    std::string get_name() const { 
      return "gradient";
    }
    int recognize(coord_type & v) {
      unsigned int sum = 0;
      
      int radius = width >> 1;
      int x = v.first, y = v.second;

      if((x > radius && x < (int)img->get_width() - radius) &&
	 (y > radius && y < (int)img->get_height() - radius)) {
	
	std::vector<double> col_sum(width), row_sum(width);


	for(int _y = -radius; _y < radius; _y++)
	  for(int _x = -radius; _x < radius; _x++) {

	    
	    rgba_pixel_t p = img->get_pixel(x + _x, y + _y);
	    double gs = RGBA_TO_GS_BY_VAL(p);

	    col_sum[_x + radius] = gs;
	    row_sum[_y + radius] = gs;
	  }
      }
      
      bool col_iter_ok = true;
      for(unsigned int i = 0; i < col_sum.size() - 1; i++) {
	if(col_sum[i] >
      }

    }

  };


*/




  /*
  class BackgroundClassifier {
  private:
    
    unsigned int wire_diameter;

    HueImageHistogram bg_hue, fg_hue;
    SaturationImageHistogram bg_sat, fg_sat;
    LightnessImageHistogram bg_l, fg_l;
    RedChannelImageHistogram bg_r, fg_r;
    GreenChannelImageHistogram bg_g, fg_g;
    BlueChannelImageHistogram bg_b, fg_b;
    HueStdDevImageHistogram bg_stdev_hue, fg_stdev_hue;
    SaturationStdDevImageHistogram bg_stdev_sat, fg_stdev_sat;
    LightnessStdDevImageHistogram bg_stdev_l, fg_stdev_l;

    
  public:

    BackgroundClassifier(unsigned int _wire_diameter) : 
      wire_diameter(_wire_diameter),
      bg_stdev_hue(wire_diameter),
      fg_stdev_hue(wire_diameter),
      bg_stdev_sat(wire_diameter),
      fg_stdev_sat(wire_diameter),
      bg_stdev_l(wire_diameter),
      fg_stdev_l(wire_diameter) {
    }

    virtual ~BackgroundClassifier() {}

    template<class ImageType>
    void add_background_areas(std::tr1::shared_ptr<ImageType> img, 
			      std::list<BoundingBox> const& bg_areas) {

      for(std::list<BoundingBox>::const_iterator iter = bg_areas.begin(); iter != bg_areas.end(); ++iter) {
	bg_hue.add_area<ImageType>(img, *iter);
	bg_sat.add_area<ImageType>(img, *iter);
	bg_l.add_area<ImageType>(img, *iter);
	bg_r.add_area<ImageType>(img, *iter);
	bg_g.add_area<ImageType>(img, *iter);
	bg_b.add_area<ImageType>(img, *iter);
	//bg_stdev_hue.add_area<ImageType>(img, *iter);
	//bg_stdev_sat.add_area<ImageType>(img, *iter);
	//bg_stdev_l.add_area<ImageType>(img, *iter);
      }
    }

    template<class ImageType>
    void add_foreground_areas(std::tr1::shared_ptr<ImageType> img, 
			      std::list<BoundingBox> const& fg_areas) {


      for(std::list<BoundingBox>::const_iterator iter = fg_areas.begin(); iter != fg_areas.end(); ++iter) {
	fg_hue.add_area<ImageType>(img, *iter);
	fg_sat.add_area<ImageType>(img, *iter);
	fg_l.add_area<ImageType>(img, *iter);
	fg_r.add_area<ImageType>(img, *iter);
	fg_g.add_area<ImageType>(img, *iter);
	fg_b.add_area<ImageType>(img, *iter);
	//fg_stdev_hue.add_area<ImageType>(img, *iter);
	//fg_stdev_sat.add_area<ImageType>(img, *iter);
	//fg_stdev_l.add_area<ImageType>(img, *iter);
      }
    }


    double get_probability(rgba_pixel_t pix) {

      double p_fg = fg_hue.get(rgba_to_hue(pix)) *
	fg_sat.get(rgba_to_saturation(pix)) *
	fg_l.get(rgba_to_lightness(pix)) *
	
	fg_r.get(MASK_R(pix)) *
	fg_g.get(MASK_G(pix)) *
	fg_b.get(MASK_B(pix))

	;

      double p_bg = bg_hue.get(rgba_to_hue(pix)) *
	bg_sat.get(rgba_to_saturation(pix)) *
	bg_l.get(rgba_to_lightness(pix)) *
	bg_r.get(MASK_R(pix)) *
	bg_g.get(MASK_G(pix)) *
	bg_b.get(MASK_B(pix))
	;

      return p_fg - p_bg;
    }

    void save_histograms(std::string const& directory) {

      bg_hue.save_histogram(join_pathes(directory, "histogram_bg_hue.dat"));
      bg_sat.save_histogram(join_pathes(directory, "histogram_bg_sat.dat"));
      bg_l.save_histogram(join_pathes(directory, "histogram_bg_l.dat"));
      bg_r.save_histogram(join_pathes(directory, "histogram_bg_r.dat"));
      bg_g.save_histogram(join_pathes(directory, "histogram_bg_g.dat"));
      bg_b.save_histogram(join_pathes(directory, "histogram_bg_b.dat"));
      //bg_stdev_hue.save_histogram(join_pathes(directory, "histogram_bg_stddev_hue.dat"));
      //bg_stdev_sat.save_histogram(join_pathes(directory, "histogram_bg_stddev_sat.dat"));
      //bg_stdev_l.save_histogram(join_pathes(directory, "histogram_bg_stddev_l.dat"));

      fg_hue.save_histogram(join_pathes(directory, "histogram_fg_hue.dat"));
      fg_sat.save_histogram(join_pathes(directory, "histogram_fg_sat.dat"));
      fg_l.save_histogram(join_pathes(directory, "histogram_fg_l.dat"));
      fg_r.save_histogram(join_pathes(directory, "histogram_fg_r.dat"));
      fg_g.save_histogram(join_pathes(directory, "histogram_fg_g.dat"));
      fg_b.save_histogram(join_pathes(directory, "histogram_fg_b.dat"));
      //fg_stdev_hue.save_histogram(join_pathes(directory, "histogram_fg_stddev_hue.dat"));
      //fg_stdev_sat.save_histogram(join_pathes(directory, "histogram_fg_stddev_sat.dat"));
      //fg_stdev_l.save_histogram(join_pathes(directory, "histogram_fg_stddev_l.dat"));

    }
    
  };
  */
}

#endif

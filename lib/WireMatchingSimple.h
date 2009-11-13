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

#ifndef __WIREMATCHINGSIMPLE_H__
#define __WIREMATCHINGSIMPLE_H__

#include <Image.h>
#include <IPPipe.h>
#include <ImageHelper.h>

#include <IPCopy.h>
#include <IPConvolve.h>
#include <IPNormalize.h>
#include <IPThresholding.h>
#include <ImageManipulation.h>
#include <IPImageWriter.h>

#include <fstream>
#include <iostream>

namespace degate {

  /**
   *
   */
  class HoughSpaceLineSegment {
  private:
    unsigned int d_min; // the distance to the intercept point
    unsigned int d_max;
    unsigned int votes;
  public:
    HoughSpaceLineSegment(unsigned int d, unsigned int _votes) :
      d_min(d),
      d_max(d),
      votes(_votes) {}
    ~HoughSpaceLineSegment() {}
    
    bool in_segment(unsigned int d) const {
      return (d_min <= d) && (d <= d_max);
    }
    
    unsigned int get_distance(unsigned int d) const {
      return abs(d - d_max);
    }
    
    void extend_d_max(unsigned int d) {
      d_max = d;
    }
    
    void vote(unsigned int votes) {
      this->votes += votes;
    }

    unsigned int get_from() const { return d_min; }
    unsigned int get_to() const { return d_max; }
    unsigned int get_votes() const { return votes; }
  };

  typedef std::tr1::shared_ptr<HoughSpaceLineSegment> HoughSpaceLineSegment_shptr;

  /**
   * A single Hough space accumulator cell.
   */
  class HoughSpaceAccumulatorCell {

  public:
    typedef  std::list<HoughSpaceLineSegment_shptr>::const_iterator line_segment_iterator;

  private:
    std::list<HoughSpaceLineSegment_shptr> line_segments;

  public:
    HoughSpaceAccumulatorCell() {}
    ~HoughSpaceAccumulatorCell() {}

    /**
     * Vote.

     * @param d This is the distance from an (x,y) in the image space to
     *   to the intercept point.
     * @param vote_count A weight for the vote.
     */
    void vote(unsigned int d, unsigned int vote_count, unsigned int max_pixel_gap) {
      
      for(std::list<HoughSpaceLineSegment_shptr>::iterator iter = line_segments.begin();
	  iter != line_segments.end(); ++iter) {

	HoughSpaceLineSegment_shptr segment = *iter;
	assert(segment != NULL);

	// find a line segment with d_min <= d <= d_max
	if(segment->in_segment(d)) {
	  segment->vote(vote_count);
	  return;
	}

	// find nearest line segment
	// if d is adjacent, extend the line segment
	else if(segment->get_distance(d) <= max_pixel_gap) {
	  segment->extend_d_max(d);
	  segment->vote(vote_count);
	  return;
	}
      }

      // else start a new line segment
      line_segments.push_back(HoughSpaceLineSegment_shptr(new HoughSpaceLineSegment(d, vote_count)));

      // sort the list (should be sorted)
    }

    line_segment_iterator begin() const {
      return line_segments.begin();
    }

    line_segment_iterator end() const {
      return line_segments.end();
    }

  };

  /**
   * A Hough space accumulator for a single phi.
   */
  class HoughSpaceAccumulator {
  private:
    std::vector<HoughSpaceAccumulatorCell> accu;
    unsigned int max_pixel_gap;
  public:
    typedef std::vector<HoughSpaceAccumulatorCell>::const_iterator accu_cell_iterator;


    HoughSpaceAccumulator(unsigned int length, unsigned int _max_pixel_gap) : 
      accu(length),
      max_pixel_gap(_max_pixel_gap) {}

    ~HoughSpaceAccumulator() {}

    /**
     * Vote.
     * @param p This is the distance from the origin of the Hough space
     *   coordinate system to the intercept point.
     * @param d This is the distance from an (x,y) in the image space to
     *   to the intercept point.
     * @param vote_count A weight for the vote.
     */
    void vote(unsigned int p, unsigned int d, unsigned int vote_count = 1) {
      assert(p < accu.size());

      unsigned int bin = p; //(p / max_pixel_gap) * max_pixel_gap;
      accu[bin].vote(d, vote_count, max_pixel_gap);
    }


    accu_cell_iterator begin() const {
      return accu.begin();
    }

    accu_cell_iterator end() const {
      return accu.end();
    }

  };

  class WireMatchingSimple {
  private:
    unsigned int min_x, max_x, min_y, max_y;
    IPPipe pipe;

    HoughSpaceAccumulator accu_0; // for vertical lines (phi = 0)
    HoughSpaceAccumulator accu_90; // for horizontal lines (phi = pi/2)

    bool match_horizontal_lines, match_vertical_lines;

    unsigned int max_pixel_gap;

    void setup_pipe(unsigned int min_x, unsigned int max_x, 
		    unsigned int min_y, unsigned int max_y,
		    bool match_horizontal_lines,
		    bool match_vertical_lines);

    void hline(TempImage_GS_DOUBLE_shptr img, 
	       unsigned int x1, unsigned int x2, unsigned int y, double c) {
      assert(y < img->get_height());
      for(unsigned int _x = x1; _x < x2; _x++) {
	assert(_x < img->get_width());
	img->set_pixel(_x, y, c);
      }
    }
    void vline(TempImage_GS_DOUBLE_shptr img, 
	       unsigned int x, unsigned int y1, unsigned int y2, double c) {
      assert(x < img->get_width());
      for(unsigned int _y = y1; _y < y2; _y++) {
	assert(_y < img->get_width());
	img->set_pixel(x, _y, c);
      }
    }

  public:

    WireMatchingSimple(unsigned int _min_x, unsigned int _max_x, 
		       unsigned int _min_y, unsigned int _max_y,
		       bool _match_horizontal_lines,
		       bool _match_vertical_lines,
		       unsigned int _max_pixel_gap = 5) :

      min_x(_min_x), 
      max_x(_max_x), 
      min_y(_min_y), 
      max_y(_max_y),
      accu_0(max_x - min_x, max_pixel_gap),
      accu_90(max_y - min_y, max_pixel_gap),
      match_horizontal_lines(_match_horizontal_lines),
      match_vertical_lines(_match_vertical_lines),
      max_pixel_gap(_max_pixel_gap) {

      setup_pipe(min_x, max_x, min_y, max_y, _match_horizontal_lines, _match_vertical_lines);
    }

    ~WireMatchingSimple() {}


    gs_double_pixel_t get_max_delta(TempImage_GS_DOUBLE_shptr img, 
				    unsigned int center_x,
				    unsigned int center_y) {
      gs_double_pixel_t deltas[8];
      gs_double_pixel_t curr = img->get_pixel_as<gs_double_pixel_t>( center_x, center_y);
      deltas[0] = center_x > 0 && center_y > 0 ?
	img->get_pixel_as<gs_double_pixel_t>( center_x - 1, center_y - 1) :
	0;
      deltas[1] = center_y > 0 ?
	img->get_pixel_as<gs_double_pixel_t>( center_x, center_y - 1) :
	0;
      deltas[2] = center_x + 1 < img->get_width() && center_y > 0 ?
	img->get_pixel_as<gs_double_pixel_t>( center_x + 1, center_y - 1) :
	0;


      deltas[3] = center_x > 0 ?
	img->get_pixel_as<gs_double_pixel_t>( center_x - 1, center_y) :
	0;
      deltas[4] = center_x + 1 < img->get_width() ?
	img->get_pixel_as<gs_double_pixel_t>( center_x + 1, center_y) :
	0;


      deltas[5] = center_x > 0 && center_y + 1 < img->get_height() ?
	img->get_pixel_as<gs_double_pixel_t>( center_x - 1, center_y + 1) :
	0;
      deltas[6] = center_y + 1 < img->get_height() ?
	img->get_pixel_as<gs_double_pixel_t>( center_x, center_y + 1) :
	0;
      deltas[7] = center_x + 1 < img->get_width() && center_y + 1 < img->get_height() ?
	img->get_pixel_as<gs_double_pixel_t>( center_x + 1, center_y + 1) :
	0;

      gs_double_pixel_t max_delta = 0;
      for(int i = 0; i < 8; i++) {
	if(fabs(deltas[i] - curr) > max_delta) max_delta = fabs(deltas[i] - curr);
      }
      return max_delta;
    }

    void run(ImageBase_shptr img_in) {


      // run the pipe
      TempImage_GS_DOUBLE_shptr img = std::tr1::dynamic_pointer_cast<TempImage_GS_DOUBLE>(pipe.run(img_in));

      // normalize
      normalize<TempImage_GS_DOUBLE>(img, 0, 1);

      
      // histogram
      std::vector<double> histo_x(img->get_width());
      std::vector<double> histo_y(img->get_height());

      for(unsigned int y = 0; y < img->get_height(); y++) {
	for(unsigned int x = 0; x < img->get_width(); x++) {
	  histo_x[x] += img->get_pixel_as<double>(x, y);
	  histo_y[y] += img->get_pixel_as<double>(x, y);
	}
      }
      std::ofstream hfilex, hfiley;
      hfilex.open("/tmp/histo_x.dat");
      hfiley.open("/tmp/histo_y.dat");

      for(std::vector<double>::const_iterator iter = histo_x.begin();
	  iter != histo_x.end(); ++iter) {
	hfilex << *iter << std::endl;
      }

      for(std::vector<double>::const_iterator iter = histo_y.begin();
	  iter != histo_y.end(); ++iter) {
	hfiley << *iter << std::endl;
      }

      hfilex.close();
      hfiley.close();


      // run a modified Hough transformation
      for(unsigned int y = 0; y < img->get_height(); y++) {
	for(unsigned int x = 0; x < img->get_width(); x++) {

	  unsigned int vote_count = 1; //255 * get_max_delta(img, x, y);
	  
	  if(img->get_pixel_as<gs_double_pixel_t>(x, y) > 0.6) {

	    
	    if(match_vertical_lines) {
	      // for vertical line votes (this is phi = 0)
	      unsigned int p_0 = x; // distance from origin to intercept point
	      unsigned int d_0 = y; // distance from intercept point to (x,y)
	      p_0 = hill_climb(histo_x, p_0);
	      accu_0.vote(p_0, d_0, vote_count); // vertical lines
	    }

	    if(match_horizontal_lines) {
	      // for horizontal line votes (this is phi = pi/2 = 90 degree)
	      unsigned int p_90 = y; // distance from origin to intercept point
	      unsigned int d_90 = x; // distance from intercept point to (x,y)
	      
	      p_90 = hill_climb(histo_y, p_90);
	      accu_90.vote(p_90, d_90, vote_count); // horizontal lines
	    }

	  }
	}
      }

      
      
      collect_results(img);      
    }
    
  private:

    unsigned int hill_climb(std::vector<double> histo, unsigned int start_index) {

      unsigned int pos = start_index;
      bool running = true;

      while(running) {
	if(pos > 0 && pos < histo.size()) {
	  if(histo[pos - 1] > histo[pos]) pos--;
	  else if(histo[pos + 1] > histo[pos]) pos++;
	  else running = false;
	}
	else running = false;
      }
      return pos;
    }

    void collect_results(TempImage_GS_DOUBLE_shptr img) {

      unsigned int x = 0, y = 0;
      
      if(match_horizontal_lines) {
	// collect horizontal lines
	for(HoughSpaceAccumulator::accu_cell_iterator iter = accu_90.begin();
	    iter != accu_90.end(); ++iter, y++) {
	  
	  HoughSpaceAccumulatorCell const& accu_cell = *iter;
	  
	  for(HoughSpaceAccumulatorCell::line_segment_iterator seg_iter = accu_cell.begin();
	      seg_iter != accu_cell.end(); ++seg_iter) {
	    
	    const HoughSpaceLineSegment_shptr segment(*seg_iter);
	    assert(segment != NULL);
	    
	    debug(TM, "horizontal line: y=%d from-x=%d to-x=%d votes=%d", 
		  y, segment->get_from(), segment->get_to(), segment->get_votes());
	    
	    if(segment->get_votes() > max_pixel_gap)
	      hline(img, segment->get_from(), segment->get_to(), y, 2);
	  }
	}
      }

      // collect vertical lines
      if(match_vertical_lines) {
	for(HoughSpaceAccumulator::accu_cell_iterator iter = accu_0.begin();
	    iter != accu_0.end(); ++iter, x++) {
	  
	  HoughSpaceAccumulatorCell const& accu_cell = *iter;
	  
	  for(HoughSpaceAccumulatorCell::line_segment_iterator seg_iter = accu_cell.begin();
	      seg_iter != accu_cell.end(); ++seg_iter) {
	    
	    const HoughSpaceLineSegment_shptr segment(*seg_iter);
	    assert(segment != NULL);
	    
	    //debug(TM, "horizontal line: y=%d from-x=%d to-x=%d votes=%d", 
	    //y, segment->get_from(), segment->get_to(), segment->get_votes());
	    
	    if(segment->get_votes() > max_pixel_gap)
	      vline(img, x, segment->get_from(), segment->get_to(), 2);
	  }
	}
      }

      save_normalized_image<TempImage_GS_DOUBLE>("/tmp/6.tif", img);
    }





  };

}

#endif

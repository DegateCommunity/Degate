#ifndef __LINESEGMENTEXTRACTION_H__
#define __LINESEGMENTEXTRACTION_H__

#include <Image.h>
#include <ImageManipulation.h>
#include <Line.h>
#include <memory>
#include <fstream>

#include <boost/foreach.hpp>

namespace degate{

  // ----------------------------------------------------------------------------------

  /**
   * Lineare primitive
   */

  class LinearPrimitive : public Line {
  public:
    enum ORIENTATION {  HORIZONTAL, VERTICAL };

  private:
    ORIENTATION orientation;

  public:

    LinearPrimitive(int from_x, int from_y, int to_x, int to_y) :
      Line(from_x, from_y, to_x, to_y, 1) {
      orientation = abs(from_x - to_x) > abs(from_y - to_y) ? HORIZONTAL : VERTICAL;
    }

    ORIENTATION get_orientation() const { return orientation; }

    void print() {
      debug(TM, "%d,%d --- %d,%d : %d",
	    get_from_x(), get_from_y(), get_to_x(), get_to_y(), get_length());
    }

  };

  typedef std::shared_ptr<LinearPrimitive> LinearPrimitive_shptr;

  // ----------------------------------------------------------------------------------

  class LineSegment;
  typedef std::shared_ptr<LineSegment> LineSegment_shptr;

  /**
   * Line segment
   */

  class LineSegment : public LinearPrimitive {
  private:
    std::list<LinearPrimitive_shptr> segments;
  public:
    LineSegment(LinearPrimitive_shptr lp) :
      LinearPrimitive(lp->get_from_x(), lp->get_from_y(), lp->get_to_x(), lp->get_to_y()) {
    }

    void merge(LineSegment_shptr seg) {
      //std::cout << "merging lines:" << std::endl;
      //print();
      //seg->print();

      Point a1 = get_p1();
      Point a2 = get_p2();
      Point b1 = seg->get_p1();
      Point b2 = seg->get_p2();

      unsigned int a1b1 = a1.get_distance(b1);
      unsigned int a1b2 = a1.get_distance(b2);
      unsigned int a2b1 = a2.get_distance(b1);
      unsigned int a2b2 = a2.get_distance(b2);

      // +-----------------+       +--------------+
      // A1                A2      B1             B2

      if(a2b1 <= a1b1 && a2b1 <= a1b2 && a2b1 <= a2b2) {
	set_p1(a1);set_p2(b2);
      }

      // +-----------------+       +--------------+
      // A2                A1      B1             B2

      else if(a1b1 <= a1b2 && a1b1 <= a2b1 && a1b1 <= a2b2) {
	set_p1(a2); set_p2(b2);
      }

      // +-----------------+       +--------------+
      // A2                A1      B2             B1

      else if(a1b2 <= a1b1 && a1b2 <= a2b1 && a1b2 <= a2b2) {
	set_p1(a2);set_p2(b1);
      }

      // +-----------------+       +--------------+
      // A1                A2      B2             B1

      else if(a2b2 <= a1b1 && a2b2 <= a1b2 && a2b2 <= a2b1) {
	set_p1(a1);set_p2(b1);
      }

      //std::cout << "Result: " << std::endl;
      //print();

    }

  };


  // ----------------------------------------------------------------------------------

  /**
   * Line segment map.
   */
  class LineSegmentMap {
  public:

    typedef std::list<LineSegment_shptr> list_type;
    typedef list_type::iterator iterator;
    typedef list_type::const_iterator const_iterator;

  private:

    list_type lines;

  public:

    LineSegmentMap() { }

    void erase(iterator iter) {
      lines.erase(iter);
    }

    size_t size() const {
      return lines.size();
    }


    void add(LineSegment_shptr segment) {
      lines.push_back(segment);
    }

    void erase(LineSegment_shptr segment) {
      iterator iter = find(lines.begin(), lines.end(), segment);
      if(iter != lines.end())
	lines.erase(iter);
    }

    iterator begin() { return lines.begin(); }
    iterator end() { return lines.end(); }
    const_iterator begin() const { return lines.begin(); }
    const_iterator end() const { return lines.end(); }

    LineSegment_shptr find_adjacent(LineSegment_shptr elem,
				    unsigned int search_radius_along,
				    unsigned int search_radius_across) const {

      BOOST_FOREACH(LineSegment_shptr elem2, *this) {
	if(elem != elem2 && elem2->get_orientation() == elem->get_orientation()) {

	  Point a1 = elem->get_p1();
	  Point a2 = elem->get_p2();
	  Point b1 = elem2->get_p1();
	  Point b2 = elem2->get_p2();


	  if(a1.get_distance(b1) <= search_radius_along ||
	     a1.get_distance(b2) <= search_radius_along ||
	     a2.get_distance(b1) <= search_radius_along ||
	     a2.get_distance(b2) <= search_radius_along) {

	    if(elem->get_orientation() == LineSegment::HORIZONTAL) {
	      int _min = std::min(a1.get_y(),
				  std::min(a2.get_y(),
					   std::min(b1.get_y(), b2.get_y())));
	      int _max = std::max(a1.get_y(),
				  std::max(a2.get_y(),
					   std::max(b1.get_y(), b2.get_y())));
	      if((unsigned int)(_max - _min) < search_radius_across) return elem2;
	    }
	    else {
	      int _min = std::min(a1.get_x(),
				  std::min(a2.get_x(),
					   std::min(b1.get_x(), b2.get_x())));
	      int _max = std::max(a1.get_x(),
				  std::max(a2.get_x(),
					   std::max(b1.get_x(), b2.get_x())));

	      if((unsigned int)(_max - _min) < search_radius_across) return elem2;
	    }
	  }
	}
      }
      return LineSegment_shptr();
    }

    void merge(unsigned int search_radius_along,
	       unsigned int search_radius_across) {

      unsigned int counter = 0;
      unsigned int max_rounds = lines.size();
      int distance = 1;
      int max_distance = search_radius_along;
      bool running = lines.size() > 0;

      while(running) {
	debug(TM, "#segments: %d", lines.size());
	running = false;

	LineSegment_shptr ls = lines.front();
	lines.pop_front();

	LineSegment_shptr ls2 = find_adjacent(ls, distance, search_radius_across);
	if(ls2 != NULL) {
	  running = true;
	  // We could check here if line segments differ in their angles
	  ls->merge(ls2);

	  iterator found = find(lines.begin(), lines.end(), ls2);
	  assert(found != lines.end());
	  lines.erase(found);

	}
	else {
	  if(counter++ < max_rounds)
	    running = true;
	  else {
	    if(distance <= max_distance) {
	      distance++;
	      counter = 0;
	      running = true;
	    }
	    else running = false;
	  }
	}

	lines.push_back(ls);
      }


    }

    void write() const {
      std::ofstream myfile;
      myfile.open ("/tmp/example.txt");

      BOOST_FOREACH(LineSegment_shptr e, *this) {
	if(e->get_length() > 0) {
	  myfile << "line "
		 << e->get_from_x() << "," << e->get_from_y()
		 << " "
		 << e->get_to_x() << "," << e->get_to_y()
		 << std::endl;
	}

      }
      myfile.close();
    }

  };

  typedef std::shared_ptr<LineSegmentMap> LineSegmentMap_shptr;

  // ----------------------------------------------------------------------------------

  template<typename ImageType>
  class LineSegmentExtraction {

  private:

    unsigned int width, height;
    std::shared_ptr<ImageType> img;
    std::shared_ptr<ImageType> processed;
    LineSegmentMap_shptr line_segments;
    unsigned int search_radius_along;
    unsigned int search_radius_across;
    unsigned int border;

  public:
  LineSegmentExtraction(std::shared_ptr<ImageType> _img,
			unsigned int _search_radius_along,
			unsigned int _search_radius_across,
			unsigned int _border) :
      width(_img->get_width()),
      height(_img->get_height()),
      img(_img),
      processed(new ImageType(width, height)),
      line_segments(new LineSegmentMap()),
      search_radius_along(_search_radius_along),
      search_radius_across(_search_radius_across),
      border(_border) {

      copy_image<ImageType, ImageType>(processed, img);
    }

    LineSegmentMap_shptr run() {
      extract_primitives();
      line_segments->merge(search_radius_along, search_radius_across);
      line_segments->write();
      return line_segments;
    }

  private:
    void extract_primitives() {
      for(unsigned int y = border; y < height - border; y++)
	for(unsigned int x = border; x < width - border; x++) {

	  if(processed->get_pixel(x, y) > 0) {
	    LinearPrimitive_shptr lp = trace_line_primitive(processed, x, y);
	    if(lp != NULL) {
	      LineSegment_shptr segment(new LineSegment(lp));
	      //segment->print();
	      line_segments->add(segment);
	    }
	  }
	}
    }

    LinearPrimitive_shptr trace_line_primitive(std::shared_ptr<ImageType> img,
					       unsigned int x, unsigned int y) {

      LinearPrimitive_shptr segment;

      unsigned int _x = x;
      while(_x < width && img->get_pixel(_x, y) > 0) _x++;

      if(_x - x > 1) {
	segment = LinearPrimitive_shptr(new LinearPrimitive(x, y, _x, y));
	_x = x;
	while(_x < width && img->get_pixel(_x, y) > 0) {
	  img->set_pixel(_x, y, 0);
	  _x++;
	}
	return segment;
      }

      unsigned int _y = y;
      while(_y < height && img->get_pixel(x, _y) > 0) _y++;

      if(_y - y > 1) {
	segment = LinearPrimitive_shptr(new LinearPrimitive(x, y, x, _y));
	_y = y;
	while(_y < height && img->get_pixel(x, _y) > 0) {
	    img->set_pixel(x, _y, 0);
	    _y++;
	  }
	  return segment;
      }

      return segment;
    }

  };
}

#endif

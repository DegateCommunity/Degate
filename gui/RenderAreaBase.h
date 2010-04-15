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

#ifndef __RENDERAREABASE_H__
#define __RENDERAREABASE_H__

#include <assert.h>
#include <iostream>
#include <math.h>

#include <BoundingBox.h>

/**
 * Abstract base class for render objects.
 *
 * Some sotes about different coordinate systems and units we use here:
 *
 * We use the term virtual area for the whole plane, the render should
 * be able to draw. To address pixels in this plane we use the term
 * "real" coordinates.
 *
 * We also have a viewport, to show a slice of the virtual area. The
 * viewport is defined using "real" coordinates.
 *
 * The viewport is mapped to a drawing window.
 *
 * The transformation of objects within the viewport to the drawing area
 * creates its own coordinate system.
 */

class RenderAreaBase {

private:

  // virtual size of rendering area
  unsigned int virtual_width, virtual_height;

  // viewport size in real coordinates
  int viewport_min_x, viewport_min_y, viewport_max_x, viewport_max_y;

  unsigned int drawing_window_width, drawing_window_height;

  double get_scaling_x() {
    return (double)get_viewport_width() / (double)get_drawing_window_width();
  }

  double get_scaling_y() {
    return (double)get_viewport_height() / (double)get_drawing_window_height();
  }

protected:

  /**
   * Update viewport.
   */
  virtual void update_viewport_dimension() = 0;

  /**
   * Update viewport.
   */
  virtual void update_virtual_dimension() = 0;


  

  double get_scaling() {
    double s = (double)get_viewport_width() / (double)get_drawing_window_width();
    /*
    std::cout << "Viewport width " << get_viewport_width() << " Window width: " << get_drawing_window_width()
	      << " s=" << s << std::endl;
    */
    return s;
  }

public:

  /**
   * The base class constructor.
   */
  RenderAreaBase() {
    viewport_min_x = viewport_min_y = 0;
    viewport_max_x = viewport_max_y = 10;
    
    virtual_width = virtual_height = 100;
  }

  /**
   * The dtor.
   */
  virtual ~RenderAreaBase() {}


  /**
   * Get the width of the virtual area in "real" pixel units.
   */

  virtual unsigned int get_virtual_width() const { return virtual_width; }

  /**
   * Get the height of the virtual area in "real" pixel units.
   */
  virtual unsigned int get_virtual_height() const { return virtual_height; }

  /**
   * Set the width of the virtual area in "real" pixel units.
   * It will call update_virtual_dimension().
   * @see update_virtual_dimension()
   */

  virtual void set_virtual_width(unsigned int w) { 
    virtual_width = w; 
    this->update_virtual_dimension();
  }

  /**
   * Set the height of the virtual area in "real" pixel units.
   * It will call update_virtual_dimension().
   * @see update_virtual_dimension()
   */

  virtual void set_virtual_height(unsigned int  h) { 
    virtual_height = h; 
    this->update_virtual_dimension();
  }

  /**
   * Set the width and height of the virtual area in "real" pixel units.
   * It will call update_virtual_dimension().
   * @see update_virtual_dimension()
   */

  virtual void set_virtual_size(unsigned int w, unsigned int  h) { 
    virtual_width = w; 
    virtual_height = h; 
    this->update_virtual_dimension();
  }

  /**
   * Set the horizontal viewport range. It will call update_viewport_dimension().
   * @see update_viewport_dimension()
   * @param min_x The minimum x-coordinate in real pixel unit.
   * @param max_x The maximum x-coordiante in real pixel unit.
   */
  virtual void set_viewport_x_range(int min_x, int max_x) {
    assert(min_x < max_x);
    viewport_min_x = min_x;
    viewport_max_x = max_x;
    this->update_viewport_dimension();
  }

  /**
   * Set the vertical viewport range. It will call update_viewport_dimension().
   * @see update_viewport_dimension()
   * @param min_y The minimum y-coordinate in real pixel unit.
   * @param max_y The maximum y-coordiante in real pixel unit.
   */
  virtual void set_viewport_y_range(int min_y, int max_y) {
    assert(min_y < max_y);
    viewport_min_y = min_y;
    viewport_max_y = max_y;
    this->update_viewport_dimension();
  }


  /**
   * Set the viewport. It will call update_viewport_dimension().
   * @see update_viewport_dimension()
   */
  virtual void set_viewport(int min_x, int min_y, int max_x, int max_y) {
    assert(min_x < max_x);
    assert(min_y < max_y);

    viewport_min_x = min_x;
    viewport_max_x = max_x;
    viewport_min_y = min_y;
    viewport_max_y = max_y;

    this->update_viewport_dimension();
  }

  /**
   * Get the minimum x-coordinate for the current viewport.
   * @return Returns the x-coordinate.
   */
  virtual int get_viewport_min_x() const { return viewport_min_x; }

  /**
   * Get the maximum x-coordinate for the current viewport.
   * @return Returns the x-coordinate.
   */
  virtual int get_viewport_max_x() const { return viewport_max_x; }

  /**
   * Get the minimum y-coordinate for the current viewport.
   * @return Returns the y-coordinate.
   */
  virtual int get_viewport_min_y() const  { return viewport_min_y; }

  /**
   * Get the maximum y-coordinate for the current viewport.
   * @return Returns the y-coordinate.
   */
  virtual int get_viewport_max_y() const { return viewport_max_y; }

  /**
   * Get the width of the viewport in "real" pixel units.
   */
  virtual int get_viewport_width() const { return viewport_max_x - viewport_min_x; }

  /**
   * Get the height of the viewport in "real" pixel units.
   */
  virtual int get_viewport_height() const { return viewport_max_y - viewport_min_y; }


  /**
   * Get the center of the viewport for x in "real" pixel units.
   */
  virtual int get_viewport_center_x() const { 
    return viewport_min_x + ((viewport_max_x - viewport_min_x) >> 1);
  }

  /**
   * Get the center of the viewport for y in "real" pixel units.
   */
  virtual int get_viewport_center_y() const { 
    return viewport_min_y + ((viewport_max_y - viewport_min_y) >> 1);
  }


  /**
   * Shift the viewport in x- and y-direction.
   * This method performs a bounds check.
   */
  virtual void shift_viewport(int delta_x, int delta_y) {

    assert(get_viewport_width() <= (int)get_virtual_width());
    assert(get_viewport_height() <= (int)get_virtual_height());

    int h = (int)virtual_height - 1;
    int w = (int)virtual_width - 1;

    int min_x = viewport_min_x + delta_x;
    int max_x = viewport_max_x + delta_x;
    int min_y = viewport_min_y + delta_y;
    int max_y = viewport_max_y + delta_y;

    if(min_x < 0) {
      max_x -= min_x;
      min_x = 0;
    }

    if(max_x >= w) {
      int t = max_x - w;
      min_x -= t;
      max_x -= t;
    }

    if(min_y < 0) {
      max_y -= min_y;
      min_y = 0;
    }

    if(max_y >= h) {
      int t = max_y - h;
      min_y -= t;
      max_y -= t;
    }

    set_viewport(min_x, min_y, max_x, max_y);
  }

  /**
   * Shift the viewport to left.
   * Method performs a bounds check.
   */
  virtual void shift_viewport_left(double part_of_page_width = 0.3) {
    shift_viewport(-1.0 * (double)get_viewport_width() * part_of_page_width, 0);
  }

  /**
   * Shift the viewport to right.
   * Method performs a bounds check.
   */
  virtual void shift_viewport_right(double part_of_page_width = 0.3) {
    shift_viewport((double)get_viewport_width() * part_of_page_width, 0);
  }

  /**
   * Shift the viewport up.
   * Method performs a bounds check.
   */
  virtual void shift_viewport_up(double part_of_page_height = 0.3) {
    shift_viewport(0, -1.0 * (double)get_viewport_height() * part_of_page_height);
  }

  /**
   * Shift the viewport down.
   * Method performs a bounds check.
   */
  virtual void shift_viewport_down(double part_of_page_height = 0.3) {
    shift_viewport(0, (double)get_viewport_height() * part_of_page_height);
  }
  
  /**
   * Get the viewport definition as bounding box.
   */
  virtual degate::BoundingBox get_viewport() const {
    return degate::BoundingBox(viewport_min_x, viewport_max_x, 
			       viewport_min_y, viewport_max_y);
  }

  virtual void set_drawing_window_width(unsigned int width) {
    drawing_window_width = width;
  }

  virtual void set_drawing_window_height(unsigned int height) {
    drawing_window_height = height;
  }

  virtual unsigned int get_drawing_window_width() const {
    return drawing_window_width;
  }

  virtual unsigned int get_drawing_window_height() const {
    return drawing_window_height;
  }

  virtual void coord_screen_to_real(unsigned int screen_x, unsigned int screen_y, 
				    unsigned int * real_x, unsigned int * real_y) {

    assert(real_x != NULL && real_y != NULL);
    if(real_x != NULL && real_y != NULL) {
      *real_x = (unsigned int)round((double)screen_x * get_scaling_x() + (double)viewport_min_x);
      *real_y = (unsigned int)round((double)screen_y * get_scaling_y() + (double)viewport_min_y);
    }
  }

  
  virtual void coord_real_to_screen(unsigned int real_x, unsigned int real_y, 
				    unsigned int * screen_x, unsigned int * screen_y) {

    assert(screen_x != NULL && screen_y != NULL);
    if(screen_x != NULL && screen_y != NULL) {
      if((int)real_x >= viewport_min_x && (int)real_x <= viewport_max_x)
	*screen_x = (unsigned int)((real_x - viewport_min_x) / get_scaling_x());
      else if((int)real_x < viewport_min_x)
	*screen_x = 0;
      else
	*screen_x = viewport_max_x - viewport_min_x;
      
      if((int)real_y >= viewport_min_y && (int)real_y <= viewport_max_y)
	*screen_y = (unsigned int)((real_y - viewport_min_y) / get_scaling_y());
      else if((int)real_y < viewport_min_y)
	*screen_y = 0;
      else
	*screen_y = viewport_max_y - viewport_min_y;
    }
  }

};

#endif

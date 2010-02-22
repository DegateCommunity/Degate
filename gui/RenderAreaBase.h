#ifndef __RENDERAREABASE_H__
#define __RENDERAREABASE_H__

#include <assert.h>
#include <iostream>

/**
 * Abstract base class for render objects.
 *
 * Some sotes about different coordinate systems and units we use here:
 *
 * We use the term virtual area for the whole plane, the render should
 * be able to draw. To address pixels in this plane we use the term
 * "real" coordinates.
 *
 * We also have a viewport, to show a splice of the virtual area. The
 * viewport is defined using "real" coordinates.
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

protected:

  /**
   * Update viewport.
   */
  virtual void update_viewport_dimension() = 0;

  /**
   * Update viewport.
   */
  virtual void update_virtual_dimension() = 0;

  // coord trans that ca be used in derived classes

  void coord_screen_to_real(unsigned int screen_x, unsigned int screen_y, 
			    unsigned int * real_x, unsigned int * real_y) {

    //std::cout << "screen clicked at " << screen_x << std::endl;

    //*real_x = (unsigned int)(screen_x * get_scaling() + viewport_min_x);
    //*real_y = (unsigned int)(screen_y * get_scaling() + viewport_min_y);

    *real_x = (unsigned int)(screen_x * get_scaling());
    *real_y = (unsigned int)(screen_y * get_scaling());

    //std::cout << "that is real " << *real_x << std::endl;
    //std::cout << "scaling " << get_scaling() << std::endl;

  }

  /*
  void coord_real_to_screen(unsigned int real_x, unsigned int real_y, 
			    unsigned int * screen_x, unsigned int * screen_y) {

    if((int)real_x >= viewport_min_x && (int)real_x <= viewport_max_x)
      *screen_x = (unsigned int)((real_x - viewport_min_x) / get_scaling());
    else if((int)real_x < viewport_min_x)
      *screen_x = 0;
    else
      *screen_x = viewport_max_x - viewport_min_x;
    
    if((int)real_y >= viewport_min_y && (int)real_y <= viewport_max_y)
      *screen_y = (unsigned int)((real_y - viewport_min_y) / get_scaling());
    else if((int)real_y < viewport_min_y)
      *screen_y = 0;
    else
      *screen_y = viewport_max_y - viewport_min_y;
  }
  */
  double get_scaling() {
    return (double)(get_viewport_width()) / (double)(get_virtual_width());
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
    update_virtual_dimension();
  }

  /**
   * Set the height of the virtual area in "real" pixel units.
   * It will call update_virtual_dimension().
   * @see update_virtual_dimension()
   */

  virtual void set_virtual_height(unsigned int  h) { 
    virtual_height = h; 
    update_virtual_dimension();
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
    update_viewport_dimension();
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
    update_viewport_dimension();
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

    update_viewport_dimension();
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


  
};

#endif

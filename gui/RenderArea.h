#ifndef __RENDERAREA_H__
#define __RENDERAREA_H__

#include "RenderAreaBase.h"

#include <gtkmm.h>
#include <goocanvasmm.h>

#include <assert.h>
#include <iostream>

class RenderArea : public Goocanvas::Canvas::Canvas,
		   public RenderAreaBase {

private:
  sigc::signal<void>  signal_adjust_scrollbars_;
  sigc::signal<void, unsigned int, unsigned int>  signal_mouse_scroll_down_;
  sigc::signal<void, unsigned int, unsigned int>  signal_mouse_scroll_up_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_click_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_double_click_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_release_;
  sigc::signal<void, unsigned int, unsigned int>  signal_mouse_motion_;

protected:

  virtual void update_viewport_dimension() {
    /*    std::cout << "Viewport changed: " 
	      << get_viewport_min_x()
	      << ".."
	      << get_viewport_max_x()
	      << " / "
	      << get_viewport_min_y()
	      << ".."
	      << get_viewport_max_y()
	      << "Scaling: " << (1.0/get_scaling())
	      << std::endl;
    */

    set_scale(1.0/get_scaling());

    scroll_to(get_viewport_min_x(),
	      get_viewport_min_y());

    Glib::PropertyProxy<double> x = property_x1();
    Glib::PropertyProxy<double> y = property_y1();
    
    std::cout << "x=" << x.get_value() << "/" << get_viewport_min_x() << std::endl;


    

    if(!signal_adjust_scrollbars_.empty()) signal_adjust_scrollbars_();
  }

  virtual void update_virtual_dimension() {
    /*
    std::cout << "Virtual changed\n"
	      << get_virtual_width()
	      << " / "
	      << get_virtual_height()
	      << std::endl;
    */
    set_bounds(0 , 0, 
	       //get_virtual_width() + get_viewport_width(), get_virtual_height() + get_viewport_height());
	       get_virtual_width() * 2, get_virtual_height() * 2);
    if(!signal_adjust_scrollbars_.empty()) signal_adjust_scrollbars_();
  }


  bool on_scroll_event(GdkEventScroll* ev) {
    
    unsigned int x, y;
    coord_screen_to_real(ev->x, ev->y, &x, &y);

    if(ev->direction == GDK_SCROLL_UP) {
      if(!signal_mouse_scroll_up_.empty()) signal_mouse_scroll_up_(x, y);
    }
    else if(ev->direction == GDK_SCROLL_DOWN) {
      if(!signal_mouse_scroll_down_.empty()) signal_mouse_scroll_down_(x, y);
    } 
    
    return true;
  }

  bool on_button_press_event(GdkEventButton * ev) {
    unsigned int x, y;
    coord_screen_to_real(ev->x, ev->y, &x, &y);
    unsigned int b = ev->button;

    if(!signal_mouse_double_click_.empty() && ev->type == GDK_2BUTTON_PRESS)
      signal_mouse_double_click_(x, y, b);
    else if(!signal_mouse_click_.empty() && ev->type == GDK_BUTTON_PRESS)
      signal_mouse_click_(x, y, b);
    
    return false;
  }

  bool on_button_release_event(GdkEventButton * ev) {
    if(!signal_mouse_release_.empty()) {
      unsigned int x, y;
      coord_screen_to_real(ev->x, ev->y, &x, &y);
      unsigned int b = ev->button;
      signal_mouse_release_(x, y, b);
    }
    return false;
  }

  bool on_motion_notify_event(GdkEventMotion * ev) {
    if(!signal_mouse_motion_.empty()) {
      unsigned int x, y;
      coord_screen_to_real(ev->x, ev->y, &x, &y);
      signal_mouse_motion_(x, y);
    }
    return false;
  }


public:
  RenderArea();

  virtual ~RenderArea();



  /**
   * Method to set a callback function, if the scrollbars are moved.
   */
  sigc::signal<void>& signal_adjust_scrollbars() {
    return signal_adjust_scrollbars_;
  }

  sigc::signal<void, unsigned int, unsigned int>& signal_mouse_scroll_down() {
    return signal_mouse_scroll_down_;
  }

  sigc::signal<void, unsigned int, unsigned int>& signal_mouse_scroll_up() {
    return signal_mouse_scroll_up_;
  }

  sigc::signal<void, unsigned int, unsigned int, unsigned int>& signal_mouse_click() {
    return signal_mouse_click_;
  }

  sigc::signal<void, unsigned int, unsigned int, unsigned int>& signal_mouse_double_click() {
    return signal_mouse_double_click_;
  }

  sigc::signal<void, unsigned int, unsigned int, unsigned int>& signal_mouse_release() {
    return signal_mouse_release_;
  }

  sigc::signal<void, unsigned int, unsigned int>& signal_mouse_motion() {
    return signal_mouse_motion_;
  }

  virtual void set_drawing_window_width(unsigned int width) {
    RenderAreaBase::set_drawing_window_width(width);
    update_virtual_dimension();
  }

  virtual void set_drawing_window_height(unsigned int height) {
    RenderAreaBase::set_drawing_window_height(height);
    update_virtual_dimension();
  }

  virtual void update_screen() {
  }
};

#endif

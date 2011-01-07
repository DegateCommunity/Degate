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

#ifndef __RENDERAREA_H__
#define __RENDERAREA_H__

#include "RenderAreaBase.h"

#include <gtkmm.h>


#include <assert.h>
#include <iostream>

class RenderArea : public Gtk::DrawingArea,
		   public RenderAreaBase {

private:
  sigc::signal<void>  signal_adjust_scrollbars_;
  sigc::signal<void, unsigned int, unsigned int> signal_mouse_scroll_down_;
  sigc::signal<void, unsigned int, unsigned int> signal_mouse_scroll_up_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_click_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_double_click_;
  sigc::signal<void, unsigned int, unsigned int, unsigned int>  signal_mouse_release_;
  sigc::signal<void, unsigned int, unsigned int> signal_mouse_motion_;


private:

  unsigned int crop_pos(gdouble i) const {
    return i <= 0 ? 0 : lrint(i);
  }

protected:

  virtual bool on_expose_event(GdkEventExpose* event);

  virtual void update_viewport_dimension();
  virtual void update_virtual_dimension();

  bool on_scroll_event(GdkEventScroll* ev) {

    unsigned int x, y;
    coord_screen_to_real(crop_pos(ev->x), crop_pos(ev->y), &x, &y);

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
    coord_screen_to_real(crop_pos(ev->x), crop_pos(ev->y), &x, &y);
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
      coord_screen_to_real(crop_pos(ev->x), crop_pos(ev->y), &x, &y);
      unsigned int b = ev->button;
      signal_mouse_release_(x, y, b);
    }
    return false;
  }

  bool on_motion_notify_event(GdkEventMotion * ev) {
    if(!signal_mouse_motion_.empty()) {
      unsigned int x, y;
      coord_screen_to_real(crop_pos(ev->x), crop_pos(ev->y), &x, &y);
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


  virtual void update_screen() = 0;

};

#endif

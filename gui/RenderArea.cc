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

#include "RenderAreaBase.h"
#include "RenderArea.h"

#include <gtkmm.h>
#include <gtkglmm.h>

#include <iostream>
#include <cstdlib>


RenderArea::RenderArea() {

  set_events(Gdk::ALL_EVENTS_MASK);
  set_flags(Gtk::CAN_FOCUS);

}

RenderArea::~RenderArea() {
}

bool RenderArea::on_expose_event(GdkEventExpose* event) {

  if(get_width() != (int)get_drawing_window_width() ||
     get_height() != (int)get_drawing_window_height())
    update_viewport_dimension(); // will trigger a screen update
  else
    update_screen();
  return true;
}


void RenderArea::update_virtual_dimension() {
  if(!signal_adjust_scrollbars_.empty()) signal_adjust_scrollbars_();
}

void RenderArea::update_viewport_dimension() {
  if(!signal_adjust_scrollbars_.empty()) signal_adjust_scrollbars_();
}

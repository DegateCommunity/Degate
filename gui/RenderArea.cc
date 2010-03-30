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

  if(get_width() != get_drawing_window_width() ||
     get_height() != get_drawing_window_height())
    update_viewport_dimension();

  //update_screen();
  return true;
}


void RenderArea::update_virtual_dimension() {
  if(!signal_adjust_scrollbars_.empty()) signal_adjust_scrollbars_();
}

void RenderArea::update_viewport_dimension() {
  if(!signal_adjust_scrollbars_.empty()) signal_adjust_scrollbars_();
}

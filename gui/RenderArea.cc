#include "RenderAreaBase.h"
#include "RenderArea.h"

#include <gtkmm.h>

#include <iostream>
#include <cstdlib>


RenderArea::RenderArea() {
  set_scale(1);
  scroll_to(0, 0);

  Glib::RefPtr<Goocanvas::Rect> rect = Goocanvas::Rect::create(10, 10, 90, 90);
  get_root_item()->add_child(rect);

  show();
  update_viewport_dimension();
  update_virtual_dimension();
}

RenderArea::~RenderArea() {
}


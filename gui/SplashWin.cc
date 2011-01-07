/*

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

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "SplashWin.h"

#include <gtkmm.h>
#include <gdkmm.h>
#include <sigc++/bind_return.h>


SplashWin::SplashWin( int delay_msec) :
  Gtk::Window(Gtk::WINDOW_TOPLEVEL) {
  char filename[PATH_MAX];
  snprintf(filename, PATH_MAX, "%s/icons/degate_splash.png", getenv("DEGATE_HOME"));

  m_image = Gdk::Pixbuf::create_from_file(filename);
  m_image_w = m_image->get_width();
  m_image_h = m_image->get_height();


  set_skip_taskbar_hint (true);
  set_position (Gtk::WIN_POS_CENTER);
  set_resizable (false);
  set_type_hint (Gdk::WINDOW_TYPE_HINT_SPLASHSCREEN);
  set_decorated (false);
  set_opacity(0.9);
  set_app_paintable (true);
  add_events (Gdk::ALL_EVENTS_MASK);
  set_size_request (m_image_w, m_image_h);

  Glib::RefPtr<Gdk::Pixmap> mask_pixmap_window1, mask_pixmap_window2;
  Glib::RefPtr<Gdk::Bitmap> mask_bitmap_window1, mask_bitmap_window2;
  m_image->render_pixmap_and_mask (mask_pixmap_window1, mask_bitmap_window1, 0);
  m_image->render_pixmap_and_mask (mask_pixmap_window2, mask_bitmap_window2, 128);
  shape_combine_mask (mask_bitmap_window2, 0, 0);

  Glib::signal_timeout().connect(sigc::bind_return(sigc::mem_fun(*this, &SplashWin::hide), false), delay_msec);
}

SplashWin::~SplashWin() {
}

bool SplashWin::on_expose_event(GdkEventExpose *event) {

  Glib::RefPtr<Gdk::Window> window = get_window();

  if(window) {
    Gtk::Allocation allocation = get_allocation();
    Cairo::RefPtr<Cairo::Context> m_cr = window->create_cairo_context();


    m_cr->set_operator (Cairo::OPERATOR_SOURCE);
    m_cr->set_source_rgba (.0, .0, .0, .0);
    m_cr->paint ();


    gdk_cairo_set_source_pixbuf (m_cr->cobj(), m_image->gobj(), 0, 0);
    m_cr->paint ();

    /*    //gdk_cairo_set_source_pixbuf (m_cr->cobj(), m_image->gobj(), 0, 0);
    //
    m_image->render_to_drawable(get_window(), get_style()->get_black_gc(),
				0, 0, m_image->get_width(), m_image->get_height(),
				m_image->get_width(), m_image->get_height(),
				Gdk::RGB_DITHER_NONE, 0, 0);

				m_cr->stroke();*/
  }
  return true;
}


void SplashWin::delay() {
}

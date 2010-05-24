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

#include "InProgressWin.h"

#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>
#include <unistd.h>
#include <assert.h>


void InProgressWin::init(Gtk::Window *parent, const Glib::ustring& title, const Glib::ustring& message) {
  running = true;

  set_title(title);
  set_default_size(220, 100);
  set_border_width(5);
  set_transient_for(*parent);
  set_modal(true);
  set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
  set_deletable(false);

  add(m_Box);

  m_Label_Message.set_label(message);
  m_Box.pack_start(m_Label_Message, Gtk::PACK_SHRINK, 10);

  m_ProgressBar.set_pulse_step(0.02);
  m_Box.pack_start(m_ProgressBar, Gtk::PACK_SHRINK, 10);

  unsigned int mseconds = 100;
  if(pc) {
    cancel_button.signal_clicked().connect(sigc::mem_fun(*this, &InProgressWin::on_cancel_button_clicked));
    m_Box.pack_start(cancel_button, Gtk::PACK_SHRINK, 10);
    mseconds = 1000;
  }

  Glib::signal_timeout().connect( sigc::mem_fun(*this, &InProgressWin::update_progress_bar), mseconds);

  show_all_children();
}


InProgressWin::InProgressWin(Gtk::Window *parent, 
			     const Glib::ustring& title, 
			     const Glib::ustring& message, 
			     degate::ProgressControl_shptr pc) : cancel_button(Gtk::Stock::CANCEL) {
  this->pc = pc;

  init(parent, title, message);
}

InProgressWin::InProgressWin(Gtk::Window *parent, 
			     const Glib::ustring& title, 
			     const Glib::ustring& message) {
  init(parent, title, message);
}

InProgressWin::~InProgressWin() {
}

void InProgressWin::close() {
  hide_all();
  running = false;
}

bool InProgressWin::update_progress_bar() {

  if(pc) {
    double progress = pc->get_progress();
    if(progress > 0) {
      m_ProgressBar.set_fraction(progress);
      m_ProgressBar.set_text(pc->get_time_left_as_string());
    }
    else {
      m_ProgressBar.pulse();
      m_ProgressBar.set_text("");
    }
  }
  else
    m_ProgressBar.pulse();

  return running;
}


void InProgressWin::on_cancel_button_clicked() {
  if(pc) pc->cancel();
}

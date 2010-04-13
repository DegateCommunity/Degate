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

#ifdef IMPL_WITH_THREAD
Glib::StaticMutex mutex = GLIBMM_STATIC_MUTEX_INIT; 
#endif


InProgressWin::InProgressWin(Gtk::Window *parent, const Glib::ustring& title, const Glib::ustring& message) {

  running = true;

#ifdef IMPL_WITH_THREAD
  thread = NULL;
#endif

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

  #ifdef IMPL_WITH_THREAD
  signal_progress_.connect(sigc::mem_fun(*this, &InProgressWin::update_progress_bar));
  assert((thread = Glib::Thread::create(sigc::mem_fun(*this, &InProgressWin::progress_thread), true)) != NULL);
  #else
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &InProgressWin::update_progress_bar), 50);
  #endif

  show_all_children();
}

InProgressWin::~InProgressWin() {
}

void InProgressWin::close() {
  hide_all();
#ifdef IMPL_WITH_THREAD
  { 
    Glib::Mutex::Lock lock(mutex);
    running = false;
  }
  if(thread) {
    puts("InProgressWin: wait for thread.");
    //if(thread->joinable()) thread->join();
    puts("after join");
  }
#else
  running = false;
#endif
}

bool InProgressWin::update_progress_bar() {
  //puts("update");
  m_ProgressBar.pulse();
  return running;
}

#ifdef IMPL_WITH_THREAD
void InProgressWin::progress_thread() {
  signal_progress_();
  while(1) {
    signal_progress_();
    Glib::usleep(50000);
    { 
      Glib::Mutex::Lock lock(mutex);
      if(!running) {
	Glib::usleep(1000000);
	return;
      }
    }
  }
}
#endif


/*                                                                              
                                                                                
This file is part of the IC reverse engineering tool degate.                    
                                                                                
Copyright 2008, 2009 by Martin Schobert                                         
                                                                                
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

#ifndef __INPROGRESSWIN_H__
#define __INPROGRESSWIN_H__

#include <gtkmm.h>

class InProgressWin : public Gtk::Window {
 public:
  InProgressWin(Gtk::Window *parent, const Glib::ustring& title, const Glib::ustring& message);
  virtual ~InProgressWin();
  void close();

 private:

  bool running;
#ifdef IMPL_WITH_THREAD
  Glib::Thread * thread;
  Glib::Dispatcher   signal_progress_;
  void progress_thread();
#endif

  Gtk::VBox m_Box;
  Gtk::Label m_Label_Message;
  Gtk::ProgressBar m_ProgressBar;

  bool update_progress_bar();
};

#endif

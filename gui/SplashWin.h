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

#ifndef __SPLASHWIN_H__
#define __SPLASHWIN_H__

#include <gtkmm.h>

class SplashWin : public Gtk::Window {
 public:
  SplashWin(int delay_msec);
  ~SplashWin();

  void delay();
 protected:
  virtual bool on_expose_event (GdkEventExpose *event);

 private:
  Glib::RefPtr<Gdk::Pixbuf>     m_image;

  unsigned int  m_image_w;
  unsigned int  m_image_h;
};


#endif

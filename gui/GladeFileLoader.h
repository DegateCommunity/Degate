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

#ifndef __GLADEFILELOADER_H__
#define __GLADEFILELOADER_H__

#include <gtkmm.h>
#include <libglademm.h>

class GladeFileLoader {

 public:

  GladeFileLoader(std::string const& glade_file, std::string const& dialog_name);
  virtual ~GladeFileLoader();

 protected:
  Glib::RefPtr<Gnome::Glade::Xml> refXml;

  Gtk::Dialog * pDialog;
};

#endif

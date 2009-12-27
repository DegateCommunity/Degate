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

#ifndef __WIREMATCHINGPARAMSWIN_H__
#define __WIREMATCHINGPARAMSWIN_H__

#include <gtkmm.h>

#include "Project.h"
#include "LogicModel.h"
#include "GladeFileLoader.h"
#include "ScalingManager.h"

class WireMatchingParamsWin : private GladeFileLoader {

 public:

  WireMatchingParamsWin(Gtk::Window *parent, 
			unsigned int wire_diameter,
			unsigned int median_filter_width,
			double sigma,
			double min_edge_magnitude);

  ~WireMatchingParamsWin();
  
  /**
   * Start the dialog window.
   * @return Returns \p true if he the user clicked Ok. It
   *   will return false if the user clicked cancel.
   */
  bool run(unsigned int * wire_diameter,
	   unsigned int * median_filter_width,
	   double * sigma,
	   double * min_edge_magnitude);
  
 private:
  Gtk::Window *parent;

  Gtk::Entry * entry_wire_diameter;
  Gtk::Entry * entry_median_filter_width;
  Gtk::Entry * entry_sigma;
  Gtk::Entry * entry_min_edge_magnitude;

  bool ok_clicked;

  // Signal handlers:
  virtual void on_ok_button_clicked();
  virtual void on_cancel_button_clicked();

};

#endif

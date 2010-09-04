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

#include "ViaMatchingParamsWin.h"
#include "ScalingManager.h"

#include <gdkmm/window.h>
#include <gtkmm/stock.h>
#include <libglademm.h>

#include <iostream>

#include <stdlib.h>
#include <assert.h>

using namespace degate;

ViaMatchingParamsWin::ViaMatchingParamsWin(Gtk::Window *parent,
					   unsigned int median_filter_width,
					   double sigma) :
  GladeFileLoader("via_matching_params.glade", "set_via_matching_params_dialog") {

  assert(parent);
  this->parent = parent;
  ok_clicked = false;

  if(get_dialog() != NULL) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    // connect signals
    get_widget("cancel_button", pButton);
    if(pButton != NULL)
      pButton->signal_clicked().connect
	(sigc::mem_fun(*this, &ViaMatchingParamsWin::on_cancel_button_clicked));
    
    get_widget("ok_button", pButton);
    if(pButton != NULL)
      pButton->signal_clicked().connect
	(sigc::mem_fun(*this, &ViaMatchingParamsWin::on_ok_button_clicked) );

    get_widget("entry_median_filter_width", entry_median_filter_width);
    if(entry_median_filter_width != NULL) {
      char txt[100];
      snprintf(txt, sizeof(txt), "%d", median_filter_width);
      entry_median_filter_width->set_text(strdup(txt));
    }

    get_widget("entry_sigma", entry_sigma);
    if(entry_sigma != NULL) {
      char txt[100];
      snprintf(txt, sizeof(txt), "%f", sigma);
      entry_sigma->set_text(strdup(txt));
    }

  }
}

ViaMatchingParamsWin::~ViaMatchingParamsWin() {
}


bool ViaMatchingParamsWin::run(unsigned int * median_filter_width,
			       double * sigma) {

  assert(median_filter_width != NULL);
  assert(sigma != NULL);

  while(true) {
    get_dialog()->run();
    if(ok_clicked) {
      
      *median_filter_width = atoi(entry_median_filter_width->get_text().c_str());
      *sigma = atof(entry_sigma->get_text().c_str());

      if(*median_filter_width >= 0 && sigma >= 0) {
	get_dialog()->hide();
	return true;
      }
    }
    else return false;
  }

  return false;
}

void ViaMatchingParamsWin::on_ok_button_clicked() {
  ok_clicked = true;
}

void ViaMatchingParamsWin::on_cancel_button_clicked() {
  ok_clicked = false;
  get_dialog()->hide();
}


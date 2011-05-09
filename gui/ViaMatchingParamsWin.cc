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
					   double threshold_match,
					   unsigned int via_diameter,
					   unsigned int merge_n_vias) :
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

    get_widget("entry_threshold_match", entry_threshold_match);
    assert(entry_threshold_match != NULL);
    if(entry_threshold_match != NULL) {
      char txt[100];
      snprintf(txt, sizeof(txt), "%.2f", threshold_match);
      entry_threshold_match->set_text(strdup(txt));
    }

    get_widget("entry_via_diameter", entry_via_diameter);
    assert(entry_via_diameter != NULL);
    if(entry_via_diameter != NULL) {
      char txt[100];
      snprintf(txt, sizeof(txt), "%d", via_diameter);
      entry_via_diameter->set_text(strdup(txt));
    }

    get_widget("entry_merge_n_vias", entry_merge_n_vias);
    assert(entry_merge_n_vias != NULL);
    if(entry_merge_n_vias != NULL) {
      char txt[100];
      snprintf(txt, sizeof(txt), "%d", merge_n_vias);
      entry_merge_n_vias->set_text(strdup(txt));
    }

  }
}

ViaMatchingParamsWin::~ViaMatchingParamsWin() {
}


bool ViaMatchingParamsWin::run(double * threshold_match,
			       unsigned int * via_diameter,
			       unsigned int * merge_n_vias) {

  assert(threshold_match != NULL);
  assert(via_diameter != NULL);
  assert(merge_n_vias != NULL);

  while(true) {
    get_dialog()->run();
    if(ok_clicked) {

      *threshold_match = atof(entry_threshold_match->get_text().c_str());
      *via_diameter = atol(entry_via_diameter->get_text().c_str());
      *merge_n_vias = atol(entry_merge_n_vias->get_text().c_str());

      if(*via_diameter >= 0 && *threshold_match >= 0 && *merge_n_vias >= 0) {
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



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

#include "TemplateMatchingParamsWin.h"
#include "ScalingManager.h"

#include <gdkmm/window.h>
#include <gtkmm/stock.h>
#include <libglademm.h>

#include <iostream>

#include <stdlib.h>
#include <assert.h>

using namespace degate;

TemplateMatchingParamsWin::TemplateMatchingParamsWin(Gtk::Window *parent,
						     degate::ScalingManager_shptr scaling_manager,
						     double threshold_hc,
						     double threshold_detection,
						     unsigned int max_step_size_search,
						     unsigned int preselected_scale_down) :
  GladeFileLoader("template_matching_params.glade", "set_template_matching_params_dialog") {

  assert(parent);
  this->parent = parent;
  ok_clicked = false;

  if(get_dialog() != NULL) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton = NULL;
    
    // connect signals
    get_widget("cancel_button", pButton);
    if(pButton != NULL)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &TemplateMatchingParamsWin::on_cancel_button_clicked));
    
    get_widget("ok_button", pButton);
    if(pButton != NULL)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &TemplateMatchingParamsWin::on_ok_button_clicked) );
  

    get_widget("hscale_threshold_hc", hscale_threshold_hc);
    if(hscale_threshold_hc != NULL) {
      hscale_threshold_hc->set_value(threshold_hc);
    }

    get_widget("hscale_threshold_detection", hscale_threshold_detection);
    if(hscale_threshold_detection != NULL) {
      hscale_threshold_detection->set_value(threshold_detection);
    }

    get_widget("entry_step_size_search", entry_step_size_search);
    if(entry_step_size_search != NULL) {
      char txt[100];
      snprintf(txt, sizeof(txt), "%d", max_step_size_search);
      entry_step_size_search->set_text(strdup(txt));
    }

    get_widget("combobox_scale_down", combobox_scale_down);
    if(combobox_scale_down != NULL) {

      m_refTreeModel_scalings = Gtk::ListStore::create(m_Columns_scalings);
      combobox_scale_down->set_model(m_refTreeModel_scalings);
      int row_number = 0;

      
      const ScalingManager<BackgroundImage>::zoom_step_list steps = 
	scaling_manager->get_zoom_steps();

      if(preselected_scale_down > steps.back()) preselected_scale_down = steps.back();

      for(ScalingManager<BackgroundImage>::zoom_step_list::const_iterator iter = steps.begin();
	  iter != steps.end(); ++iter, row_number++) {

	unsigned int i = *iter;
	Gtk::TreeModel::Row row = *(m_refTreeModel_scalings->append());
	row[m_Columns_scalings.m_col_scaling] = i;
	if(i == preselected_scale_down) combobox_scale_down->set_active(row_number);

      }
      combobox_scale_down->pack_start(m_Columns_scalings.m_col_scaling);
    }


    get_widget("combobox_tmpl_orientations", combobox_tmpl_orientations);
    if(combobox_tmpl_orientations != NULL) {
      m_refTreeModel_orientations = Gtk::ListStore::create(m_Columns_orientations);
      combobox_tmpl_orientations->set_model(m_refTreeModel_orientations);

      populate_orientations_combobox();

      combobox_tmpl_orientations->pack_start(m_Columns_orientations.m_col_orientation_str);
      combobox_tmpl_orientations->set_active(0);
      
    }

  }
}

TemplateMatchingParamsWin::~TemplateMatchingParamsWin() {
}

void TemplateMatchingParamsWin::populate_orientations_combobox() {
  std::list<degate::Gate::ORIENTATION> o_list;

  Gtk::TreeModel::Row row;
      
  o_list.push_back(Gate::ORIENTATION_NORMAL);
  o_list.push_back(Gate::ORIENTATION_FLIPPED_UP_DOWN);
  o_list.push_back(Gate::ORIENTATION_FLIPPED_LEFT_RIGHT);
  o_list.push_back(Gate::ORIENTATION_FLIPPED_BOTH);
  row = *(m_refTreeModel_orientations->append());
  row[m_Columns_orientations.m_col_orientation_str] = "any";
  row[m_Columns_orientations.m_col_orientations] = o_list;
	
  
  o_list.clear();
  o_list.push_back(Gate::ORIENTATION_NORMAL);
  row = *(m_refTreeModel_orientations->append());
  row[m_Columns_orientations.m_col_orientation_str] = "normal";
  row[m_Columns_orientations.m_col_orientations] = o_list;

  o_list.clear();
  o_list.push_back(Gate::ORIENTATION_FLIPPED_LEFT_RIGHT);
  row = *(m_refTreeModel_orientations->append());
  row[m_Columns_orientations.m_col_orientation_str] = "flipped left-right";
  row[m_Columns_orientations.m_col_orientations] = o_list;

  o_list.clear();
  o_list.push_back(Gate::ORIENTATION_FLIPPED_UP_DOWN);
  row = *(m_refTreeModel_orientations->append());
  row[m_Columns_orientations.m_col_orientation_str] = "flipped up-down";
  row[m_Columns_orientations.m_col_orientations] = o_list;

  o_list.clear();
  o_list.push_back(Gate::ORIENTATION_FLIPPED_BOTH);
  row = *(m_refTreeModel_orientations->append());
  row[m_Columns_orientations.m_col_orientation_str] = "flipped both";
  row[m_Columns_orientations.m_col_orientations] = o_list;
  
}

bool TemplateMatchingParamsWin::run(double * threshold_hc,
				    double * threshold_detection,
				    unsigned int * max_step_size_search,
				    unsigned int * scale_down,
				    std::list<Gate::ORIENTATION> & tmpl_orientations) {
  assert(threshold_hc != NULL);
  assert(threshold_detection != NULL);
  assert(max_step_size_search != NULL);
  assert(scale_down != NULL);

  *max_step_size_search = 0;
  *scale_down = 0;
  
  while(*max_step_size_search == 0 || *scale_down == 0) {

    get_dialog()->run();
    if(ok_clicked) {

      *max_step_size_search = atoi(entry_step_size_search->get_text().c_str());

      *threshold_hc = hscale_threshold_hc->get_value();
      *threshold_detection = hscale_threshold_detection->get_value();

      Gtk::TreeModel::iterator iter;
      
      iter = combobox_tmpl_orientations->get_active();
      if(iter) {
	Gtk::TreeModel::Row row = *iter;
	if(row) {
	  tmpl_orientations = row[m_Columns_orientations.m_col_orientations];
	}
      }

      iter = combobox_scale_down->get_active();
      if(iter) {
	Gtk::TreeModel::Row row = *iter;
	if(row) {
	  *scale_down = row[m_Columns_scalings.m_col_scaling];
	}
      }

      if(*max_step_size_search > 0 && *scale_down > 0) {
	get_dialog()->hide();
	return true;
      }

    }
    else return false;
  }

  return false;
}

void TemplateMatchingParamsWin::on_ok_button_clicked() {
  ok_clicked = true;
}

void TemplateMatchingParamsWin::on_cancel_button_clicked() {
  ok_clicked = false;
  get_dialog()->hide();
}


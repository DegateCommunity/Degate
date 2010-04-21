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

#include "PortColorsWin.h"

#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>
#include <libglademm.h>
#include <assert.h>
#include <stdlib.h>

#include "Image.h"

using namespace degate;

PortColorsWin::PortColorsWin(Gtk::Window *parent, 
			     PortColorManager_shptr pcm) :
  GladeFileLoader("port_colors.glade", "port_colors_dialog") {

  this->pcm = pcm;
  this->parent = parent;
  
  if(pDialog) {
    //Get the Glade-instantiated Button, and connect a signal handler:
    Gtk::Button* pButton;

    // connect signals
    refXml->get_widget("close_button", pButton);
    if(pButton)
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &PortColorsWin::on_close_button_clicked));
    
    refXml->get_widget("add_button", pButton);
    if(pButton) {
      pButton->signal_clicked().connect(sigc::mem_fun(*this, &PortColorsWin::on_add_button_clicked) );
    }
    
    refXml->get_widget("remove_button", pRemoveButton);
    if(pRemoveButton) {
      pRemoveButton->signal_clicked().connect(sigc::mem_fun(*this, &PortColorsWin::on_remove_button_clicked) );
      pRemoveButton->set_sensitive(false);
    }
    
    refXml->get_widget("edit_button", pEditButton);
    if(pEditButton) {
      pEditButton->signal_clicked().connect(sigc::mem_fun(*this, &PortColorsWin::on_edit_button_clicked) );
      pEditButton->set_sensitive(false);
    }
    
    refListStore = Gtk::ListStore::create(m_Columns);
    
    
    refXml->get_widget("treeview", pTreeView);
    if(pTreeView) {
      
      Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = pTreeView->get_selection();
      refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &PortColorsWin::on_selection_changed));
      
      pTreeView->set_model(refListStore);
      pTreeView->append_column_editable("Port Name", m_Columns.m_col_port_name);
      
      Gtk::TreeView::Column * pColumn;
      
      pColumn = pTreeView->get_column(0);
      if(pColumn) pColumn->set_sort_column(m_Columns.m_col_port_name);
      
      Gtk::CellRendererText * pRenderer = Gtk::manage( new Gtk::CellRendererText()); 
      pTreeView->append_column("Color", *pRenderer);
      pColumn = pTreeView->get_column(1);
      pColumn->add_attribute(*pRenderer, "background-gdk", m_Columns.color_); 
      
    }
    
    for(PortColorManager::port_color_collection::iterator iter = pcm->begin();
	iter != pcm->end(); ++iter) {
      
      Gtk::TreeModel::Row row = *(refListStore->append()); 
    
      std::string port_name = (*iter).first;
      row[m_Columns.m_col_port_name] = port_name;
      row[m_Columns.color_] = get_color(pcm->get_fill_color(port_name));
    }
    
  }
  else {
    std::cout << "Error: can't find port_color_dialog" << std::endl;
  }
}

Gdk::Color PortColorsWin::get_color(degate::color_t col) {

  Gdk::Color c;

  c.set_red(MASK_R(col) << 8);
  c.set_green(MASK_G(col) << 8);
  c.set_blue(MASK_B(col) << 8);

  return c;
}

PortColorsWin::~PortColorsWin() {
}

void PortColorsWin::run() {
  pDialog->run();
}

void PortColorsWin::on_close_button_clicked() {

  typedef Gtk::TreeModel::Children type_children;

  type_children children = refListStore->children();
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter) {
    Gtk::TreeModel::Row row = *iter;

    Gdk::Color c = row[m_Columns.color_];
    color_t col = MERGE_CHANNELS(c.get_red() >> 8 , 
				 c.get_green() >> 8, 
				 c.get_blue() >> 8, 0xff);

    Glib::ustring port_name = row[m_Columns.m_col_port_name];

    pcm->set_color(port_name, col, col);
  }

  pDialog->hide();
}

void PortColorsWin::on_add_button_clicked() {
  Gtk::TreeModel::Row row = *(refListStore->append()); 
  row[m_Columns.m_col_port_name] = "click to edit";
  row[m_Columns.color_] = get_color(0x7fb006b2);
}


void PortColorsWin::on_remove_button_clicked() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter; 

      Gtk::MessageDialog dialog(*parent, "Are you sure you want to remove the color setting?", 
				true, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      dialog.set_title("Warning");      
      if(dialog.run() == Gtk::RESPONSE_YES) {
	dialog.hide();
	
	Glib::ustring port_name = row[m_Columns.m_col_port_name];

	pcm->remove_color(port_name);

	refListStore->erase(iter);
      }

    }
  }

}

void PortColorsWin::on_edit_button_clicked() {
  

  Gtk::ColorSelectionDialog dialog("Select a color");

  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      Gtk::TreeModel::Row row = *iter; 

      Glib::ustring port_name = row[m_Columns.m_col_port_name];
      Gdk::Color col = row[m_Columns.color_];

      Gtk::ColorSelection* pColorSel = dialog.get_colorsel();
      pColorSel->set_current_color(col);
      pColorSel->set_has_opacity_control(true);

      pColorSel = dialog.get_colorsel();
      int result = dialog.run();
      if(result == Gtk::RESPONSE_OK) {
	col = pColorSel->get_current_color();

	row[m_Columns.color_] = col;
	
	Glib::ustring port_name = row[m_Columns.m_col_port_name];
	color_t _col = MERGE_CHANNELS(col.get_red() >> 8, 
				      col.get_green() >> 8, 
				      col. get_blue() >> 8,
				      pColorSel->get_current_alpha() >> 8);
	pcm->set_color(port_name, _col, _col);
      }

    }
  }



  // unselect
}


void PortColorsWin::on_selection_changed() {
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection =  pTreeView->get_selection();
  if(refTreeSelection) {
    Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
    if(*iter) {
      pEditButton->set_sensitive(true);
      pRemoveButton->set_sensitive(true);
    }
  }
}

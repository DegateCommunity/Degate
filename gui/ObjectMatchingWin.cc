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

#include "ObjectMatchingWin.h"

#include <gdkmm/window.h>
#include <iostream>
#include <gtkmm/stock.h>

using namespace degate;

ObjectMatchingWin::ObjectMatchingWin(Gtk::Window *parent) : 
  m_Frame_MatchHV("Match objects"),

  m_Frame_PinDia("Pin diameter"),

  m_Frame_Threshold("Color separation threshold"),

  m_Button_MatchH("Match horizontal objects"),
  m_Button_MatchV("Match vertical objects"),

  // value, lower, upper, step_increment
  m_Adjustment_Threshold(128.0, 0.0, 256.0, 1),
  m_Scale_Threshold(m_Adjustment_Threshold),

  m_Adjustment_PinDiameter(4.0, 0.0, 20.0, 1),
  m_Scale_PinDiameter(m_Adjustment_PinDiameter),

  m_Button_Cancel("Cancel"),
  m_Button_Ok("Ok")

{

  set_title("Object Matching");
  set_default_size(220, 200);
  set_border_width(5);
  set_transient_for(*parent);

  add(m_Box);

  m_Button_MatchV.signal_clicked().connect(sigc::mem_fun(*this, &ObjectMatchingWin::on_button_match_vo_clicked));
  m_Button_MatchH.signal_clicked().connect(sigc::mem_fun(*this, &ObjectMatchingWin::on_button_match_ho_clicked));
  m_Box.pack_start(m_Frame_MatchHV, Gtk::PACK_EXPAND_WIDGET);
  m_Frame_MatchHV.add(m_Box_MatchHV);
  m_Box_MatchHV.add(m_Button_MatchH);
  m_Box_MatchHV.add(m_Button_MatchV);
  m_Button_MatchH.set_active(true);
  m_Button_MatchV.set_active(true);

  m_Box.pack_start(m_Frame_Threshold, Gtk::PACK_EXPAND_WIDGET);
  m_Frame_Threshold.add(m_Scale_Threshold);
  m_Adjustment_Threshold.signal_value_changed().connect(sigc::mem_fun(*this,
								      &ObjectMatchingWin::on_col_sep_threshold_changed));

  m_Box.pack_start(m_Frame_PinDia, Gtk::PACK_EXPAND_WIDGET);
  m_Frame_PinDia.add(m_Scale_PinDiameter);
  m_Adjustment_PinDiameter.signal_value_changed().connect(sigc::mem_fun(*this,
									&ObjectMatchingWin::on_pin_diameter_changed));
 


  m_Button_Cancel.signal_clicked().connect( sigc::mem_fun(*this, &ObjectMatchingWin::on_cancel_button_clicked) );
  m_Button_Cancel.set_label("Cancel");
  m_Button_Cancel.set_use_stock(true);
  m_Button_Cancel.set_relief(Gtk::RELIEF_NORMAL);

  m_Button_Ok.signal_clicked().connect( sigc::mem_fun(*this, &ObjectMatchingWin::on_ok_button_clicked) );
  m_Button_Ok.set_label("Ok");
  m_Button_Ok.set_use_stock(true);
  m_Button_Ok.set_relief(Gtk::RELIEF_NORMAL);

  
  m_Box_Buttons.pack_start(m_Button_Cancel, Gtk::PACK_EXPAND_WIDGET);
  m_Box_Buttons.pack_start(m_Button_Ok, Gtk::PACK_EXPAND_WIDGET);

  m_Box.pack_start(m_Box_Buttons, Gtk::PACK_SHRINK);

  show_all_children();
}

ObjectMatchingWin::~ObjectMatchingWin() {
  std::cout << "destroy" << std::endl;
}

void ObjectMatchingWin::on_button_match_ho_clicked() {
  signal_changed_();
}

void ObjectMatchingWin::on_button_match_vo_clicked() {
  signal_changed_();
}

void ObjectMatchingWin::on_col_sep_threshold_changed() {
  signal_changed_();
}

void ObjectMatchingWin::on_pin_diameter_changed() {
  signal_changed_();
}

int ObjectMatchingWin::get_status() {
  return status;
}

void ObjectMatchingWin::on_ok_button_clicked() {
  status = Gtk::RESPONSE_OK;
  hide();
}

void ObjectMatchingWin::on_cancel_button_clicked() {
  status = Gtk::RESPONSE_CANCEL;
  hide();
}


unsigned int ObjectMatchingWin::get_threshold() {
  return (unsigned int) m_Adjustment_Threshold.get_value();
}

unsigned int ObjectMatchingWin::get_pin_diameter() {
  return (unsigned int) m_Adjustment_PinDiameter.get_value();
}

bool ObjectMatchingWin::get_horizontal_match_flag() {
  return m_Button_MatchH.get_active() ? true : false;
}

bool ObjectMatchingWin::get_vertical_match_flag() {
  return m_Button_MatchV.get_active() ? true : false;
}

sigc::signal<void>& ObjectMatchingWin::signal_changed() {
  return signal_changed_;
}

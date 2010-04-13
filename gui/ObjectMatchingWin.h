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

#ifndef __OBJECTMATCHINGWIN_H__
#define __OBJECTMATCHINGWIN_H__

#include <gtkmm.h>

#include "Project.h"

class ObjectMatchingWin : public Gtk::Window {
 public:

  ObjectMatchingWin(Gtk::Window *parent);
  ~ObjectMatchingWin();

  unsigned int get_threshold();
  unsigned int get_pin_diameter();
  bool get_horizontal_match_flag();
  bool get_vertical_match_flag();

  sigc::signal<void>& signal_changed();

  int get_status();

  private:

  int status;
  sigc::signal<void>  signal_changed_;

  // Child widgets

  Gtk::VBox m_Box;

  Gtk::Frame m_Frame_MatchHV;
  Gtk::VBox  m_Box_MatchHV;

  Gtk::Frame m_Frame_PinDia;

  Gtk::Frame m_Frame_Threshold;

  Gtk::CheckButton m_Button_MatchH;
  Gtk::CheckButton m_Button_MatchV;

  Gtk::Adjustment m_Adjustment_Threshold;
  Gtk::HScale m_Scale_Threshold;

  Gtk::Adjustment m_Adjustment_PinDiameter;
  Gtk::HScale m_Scale_PinDiameter;


  Gtk::HBox  m_Box_Buttons;
  Gtk::Button m_Button_Cancel;
  Gtk::Button m_Button_Ok;

  // Signal handlers:
  virtual void on_button_match_ho_clicked();
  virtual void on_button_match_vo_clicked();
  virtual void on_col_sep_threshold_changed();
  virtual void on_pin_diameter_changed();
  virtual void on_ok_button_clicked();
  virtual void on_cancel_button_clicked();
};

#endif

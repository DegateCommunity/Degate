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

#ifndef __INPROGRESSWIN_H__
#define __INPROGRESSWIN_H__

#include <gtkmm.h>
#include <ProgressControl.h>

class InProgressWin : public Gtk::Window {
 public:
  InProgressWin(Gtk::Window *parent, const Glib::ustring& title, const Glib::ustring& message);
  InProgressWin(Gtk::Window *parent, const Glib::ustring& title, const Glib::ustring& message,
		degate::ProgressControl_shptr pc);
  virtual ~InProgressWin();
  void close();

 private:

  degate::ProgressControl_shptr pc;

  bool running;

  Gtk::VBox m_Box;
  Gtk::Label m_Label_Message;
  Gtk::ProgressBar m_ProgressBar;
  Gtk::Button cancel_button;

  bool update_progress_bar();

  void init(Gtk::Window *parent, const Glib::ustring& title, const Glib::ustring& message);

  void on_cancel_button_clicked();
};

#endif

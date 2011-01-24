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

#ifndef __TERMINALWIN_H__
#define __TERMINALWIN_H__

#include <globals.h>
#include <degate.h>
#include "GladeFileLoader.h"
#include <gtkmm.h>

#include <list>

class TerminalWin : private GladeFileLoader {

 public:

  TerminalWin(Gtk::Window * parent = NULL);

  virtual ~TerminalWin();

  /**
   * Display the window.
   */
  void show();


 private:

  Gtk::Window *parent;

  Gtk::Button* pCloseButton;

  int fd_stdin, fd_stdout; //file descriptors
  Glib::Pid pid;
  Gtk::TextView * textview;

  void exec_program();

  // Signal handlers:
  virtual void on_close_button_clicked();
  virtual bool on_read(Glib::IOCondition condition);

};

#endif

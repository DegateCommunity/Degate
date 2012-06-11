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
#include <memory>

class TerminalWin : private GladeFileLoader {

 public:

  typedef std::list<std::string> cmd_type;

  TerminalWin(Gtk::Window * parent = NULL);

  virtual ~TerminalWin();

  virtual void run(cmd_type const& cmd, std::string const & working_dir = "");
  virtual void run(std::list<cmd_type> const& cmd, std::string const & working_dir = "");


 private:

  Gtk::Window *parent;

  Gtk::Button* pCloseButton;

  int fd_stdin, fd_stdout, fd_stderr; //file descriptors
  Glib::Pid pid;
  Gtk::TextView * textview;

  std::string buf_stdout, buf_stderr;

  std::list<cmd_type> cmds; // list of commands

  std::string working_dir;

  enum CMD_STATE {
    DONE = 0,
    FAILED = 1,
    RUNNING = 2
  } cmd_state;
  
 private:

  void run_next_command();
  void exec_program(cmd_type cmd);

  bool handle_io(Glib::IOCondition condition, std::string & strbuf);
  bool read_and_append(int fd, std::string & strbuf);
  void append_text(std::string const& s);

  // Signal handlers:
  virtual void on_close_button_clicked();
  virtual bool on_read_stdout(Glib::IOCondition condition);
  virtual bool on_read_stderr(Glib::IOCondition condition);


};

typedef std::shared_ptr<TerminalWin> TerminalWin_shptr;

#endif

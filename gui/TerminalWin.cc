/* -*-c++-*-

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

#include "TerminalWin.h"


#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>


using namespace degate;

TerminalWin::TerminalWin(Gtk::Window * parent) :
  GladeFileLoader("terminal.glade", "terminal_win") {

  if(get_dialog()) {
    
    // connect signals
    get_widget("close_button", pCloseButton);
    assert(pCloseButton != NULL);
    if(pCloseButton != NULL)
      pCloseButton->signal_clicked().connect(sigc::mem_fun(*this, &TerminalWin::on_close_button_clicked));
    

    get_widget("textview", textview);
    assert(textview != NULL);
    if(textview != NULL) {
      textview->modify_font(Pango::FontDescription("courier"));
    }

  }
}



TerminalWin::~TerminalWin() {
  Glib::spawn_close_pid(pid);
}




void TerminalWin::show() {
  exec_program();
  get_dialog()->show();
}


void TerminalWin::on_close_button_clicked() {
  get_dialog()->hide();
}


void TerminalWin::exec_program() {

  std::vector< std::string > envp(0);
  std::vector< std::string > argv(2);
  argv[0] = "ls";
  argv[1] = "-l";

  try {

    Glib::spawn_async_with_pipes(Glib::get_current_dir(), 
				 argv,
				 Glib::SPAWN_SEARCH_PATH, 
				 sigc::slot< void >(), // const sigc::slot< void > &  child_setup = sigc::slot< void >(),
				 &pid, 
				 NULL, &fd_stdout, NULL); 
    
    // create the IOChannel from the file descriptors
    //ch_stdin=Glib::IOChannel::create_from_fd(fd_stdin);
    //ch_stdout=Glib::IOChannel::create_from_fd(fd_stdout);
    
    //  Glib::ustring input,output; 
    
    Glib::signal_io().connect(sigc::mem_fun(*this, &TerminalWin::on_read), 
			      fd_stdout, 
			      Glib::IO_IN);
    

  }
  catch(Glib::SpawnError const& ex) {
    boost::format f("Failed to launch command \"%1%\". Error was: \"%2%\"");
    f % boost::algorithm::join(argv, " ") % ex.what().c_str();

    Gtk::MessageDialog dialog(f.str(), true, Gtk::MESSAGE_ERROR);
    dialog.set_title("Execution Error");
    dialog.run();
  }
}


bool TerminalWin::on_read(Glib::IOCondition condition) {
  std::cout << "on_read()" << std::endl;

  if(condition & Glib::IO_IN) {
    char buf[1024];
    int n = read(fd_stdout, buf, sizeof(buf));

    textview->get_buffer()->insert(textview->get_buffer()->end(), buf);

    return (n > 0);
  }
  else if((condition & Glib::IO_NVAL) ||
	  (condition & Glib::IO_ERR) ||
	  (condition & Glib::IO_HUP)) {
    return false;
  }

  return false;
}

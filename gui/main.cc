/*

This file is part of the IC reverse engineering tool degate.

Copyright 2008, 2009, 2010 by Martin Schobert
Copyright 2012 Robert Nitsch

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

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include "FileSystem.h"
#include <gtkglmm.h>

#include "MainWin.h"
#include "SplashWin.h"

using namespace degate;

void show_env_help() {
    std::cout
      << std::endl
      << "Error: Environment variable DEGATE_HOME is undefined. Please set it, e.g. "
      << std::endl
      << std::endl
      << "\texport DEGATE_HOME=/home/foo/degate"
      << std::endl
      << std::endl
      << std::endl
      << std::endl;
}

bool directory_exists(const char * const env_variable, const char * const sub_path) {
  char path[PATH_MAX];
  assert(env_variable != NULL);
  assert(sub_path != NULL);
  if(env_variable == NULL || sub_path == NULL) return false;

  snprintf(path, PATH_MAX, "%s/%s", getenv(env_variable), sub_path);

  if(!is_directory(path)) {
    std::cout << "Error: your environment variable " << env_variable
	      << " seems to be incorrect. The directory " << path
	      << " does not exist."
	      << std::endl;
    return false;
  }
  else return true;

}


int main(int argc, char ** argv) {

  // check environment variables
  if(getenv("DEGATE_HOME") == NULL) {
    show_env_help();
    exit(1);
  }

  if(directory_exists("DEGATE_HOME", "glade") == false) exit(1);
  if(directory_exists("DEGATE_HOME", "icons") == false) exit(1);

  // setup threading
  if(!Glib::thread_supported()) Glib::thread_init();
  Gtk::Main kit(argc, argv);

  // setup splash window
  SplashWin * splashWin = new SplashWin(1500);
  Gtk::Main::run(*splashWin);
  setlocale(LC_ALL, "C");

  // Init gtkglextmm.
  Gtk::GL::init(argc, argv);

  // create main window
  MainWin mainWin;
  if(argc > 1 && argv[1] != NULL) mainWin.set_project_to_open(argv[1]);

  delete splashWin;

  Gtk::Main::run(mainWin);


  return 1;
}

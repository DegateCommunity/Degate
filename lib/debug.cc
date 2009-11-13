/*
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
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

#include "globals.h"

#include <stdio.h>
#include <stdarg.h>

using namespace degate;

#ifdef DEBUG
void debug(const char * const module, int line, const char * const format, ...) {

  va_list args;
  printf("[%s:%d] ", module, line);

  va_start(args, format);
  vfprintf(stdout, format, args);
  va_end(args);
  puts("");

}

#endif


std::string degate::gen_tabs(int n) {
  std::string ret;
  for(int i = 0; i < n; i++)
    ret += "\t";
  return ret;
}

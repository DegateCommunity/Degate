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

#include <RCBase.h>
#include <RCVContainer.h>

using namespace degate;

RCVContainer::RCVContainer() {
}

RCVContainer::~RCVContainer() {
}

void RCVContainer::push_back(RCViolation_shptr rcv) {
  violations.push_back(rcv);
}

RCVContainer::iterator RCVContainer::begin() { 
  return violations.begin(); 
}

RCVContainer::iterator RCVContainer::end() { 
  return violations.end(); 
}

RCVContainer::const_iterator RCVContainer::begin() const { 
  return violations.begin(); 
}

RCVContainer::const_iterator RCVContainer::end() const { 
  return violations.end(); 
}

void RCVContainer::clear() { 
  violations.clear(); 
}

size_t RCVContainer::size() const { 
  return violations.size(); 
}

bool RCVContainer::contains(RCViolation_shptr rcv) const { 
  return find(rcv) != end();
}

bool RCVContainer::erase(RCViolation_shptr rcv) {
  iterator iter = find(rcv);
  if(iter != end()) {
    violations.erase(iter);
    return true;
  }
  return false;
}

RCVContainer::iterator RCVContainer::find(RCViolation_shptr rcv) {
  for(iterator iter = begin(); iter != end(); ++iter) {
    if((*iter)->equals(rcv)) return iter;
  }
  return end();
}

RCVContainer::const_iterator RCVContainer::find(RCViolation_shptr rcv) const {
  for(const_iterator iter = begin(); iter != end(); ++iter) {
    if((*iter)->equals(rcv)) return iter;
  }
  return end();
}

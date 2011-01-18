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

#include <DRCBase.h>
#include <DRCVContainer.h>

using namespace degate;

DRCVContainer::DRCVContainer() {
}

DRCVContainer::~DRCVContainer() {
}

void DRCVContainer::push_back(DRCViolation_shptr drcv) {
  violations.push_back(drcv);
}

DRCVContainer::iterator DRCVContainer::begin() { 
  return violations.begin(); 
}

DRCVContainer::iterator DRCVContainer::end() { 
  return violations.end(); 
}

DRCVContainer::const_iterator DRCVContainer::begin() const { 
  return violations.begin(); 
}

DRCVContainer::const_iterator DRCVContainer::end() const { 
  return violations.end(); 
}

void DRCVContainer::clear() { 
  violations.clear(); 
}

size_t DRCVContainer::size() const { 
  return violations.size(); 
}

bool DRCVContainer::contains(DRCViolation_shptr drcv) const { 
  return find(drcv) != end();
}

bool DRCVContainer::erase(DRCViolation_shptr drcv) {
  iterator iter = find(drcv);
  if(iter != end()) {
    violations.erase(iter);
    return true;
  }
  return false;
}

DRCVContainer::iterator DRCVContainer::find(DRCViolation_shptr drcv) {
  for(iterator iter = begin(); iter != end(); ++iter) {
    if((*iter)->equals(drcv)) return iter;
  }
  return end();
}

DRCVContainer::const_iterator DRCVContainer::find(DRCViolation_shptr drcv) const {
  for(const_iterator iter = begin(); iter != end(); ++iter) {
    if((*iter)->equals(drcv)) return iter;
  }
  return end();
}

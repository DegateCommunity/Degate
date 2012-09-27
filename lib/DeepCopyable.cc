/* -*-c++-*-

 This file is part of the IC reverse engineering tool degate.

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

#include "DeepCopyable.h"

#include <assert.h>

namespace degate {

DeepCopyable_shptr DeepCopyable::cloneDeep(oldnew_t *oldnew) const {
  auto _this = shared_from_this();
  if (cloneOnce(_this, oldnew)) {
    cloneDeepInto((*oldnew)[_this], oldnew);
  }
  return (*oldnew)[_this];
}

bool DeepCopyable::cloneOnce(const c_DeepCopyable_shptr &o, std::map<c_DeepCopyable_shptr, DeepCopyable_shptr> *__oldnew) {
  assert(o.get() != 0);
  
  std::map<c_DeepCopyable_shptr, DeepCopyable_shptr> &oldnew = *__oldnew;
  if (oldnew.count(o) > 0) {
    return false;
  }
  
  DeepCopyable_shptr clone = o->cloneShallow();
  oldnew[o] = clone;
  return true;
}

}

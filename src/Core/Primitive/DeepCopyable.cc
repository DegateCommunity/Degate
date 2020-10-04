/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2012 Robert Nitsch
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "DeepCopyable.h"

#include <cassert>

namespace degate
{
    DeepCopyable_shptr DeepCopyable::clone_deep(oldnew_t* oldnew) const
    {
        auto _this = shared_from_this();
        if (clone_once(_this, oldnew))
        {
            clone_deep_into((*oldnew)[_this], oldnew);
        }
        return (*oldnew)[_this];
    }

    bool DeepCopyable::clone_once(const c_DeepCopyable_shptr& o,
                                  std::map<c_DeepCopyable_shptr, DeepCopyable_shptr>* oldnew)
    {
        assert(o.get() != nullptr);

        std::map<c_DeepCopyable_shptr, DeepCopyable_shptr>& oldnew_ref = *oldnew;
        if (oldnew_ref.count(o) > 0)
        {
            return false;
        }

        DeepCopyable_shptr clone = o->clone_shallow();
        oldnew_ref[o] = clone;
        return true;
    }
}

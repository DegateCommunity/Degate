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

#ifndef DEEPCOPYABLE_H
#define	DEEPCOPYABLE_H

#include <map>
#include <memory>

namespace degate {

class DeepCopyable;
typedef std::shared_ptr<DeepCopyable> DeepCopyable_shptr;
typedef std::shared_ptr<const DeepCopyable> c_DeepCopyable_shptr;

class DeepCopyableBase {
public:
  typedef std::map<c_DeepCopyable_shptr, DeepCopyable_shptr> oldnew_t;
protected:
  /**
   * @brief Deep-copy all members to \a destination.
   * 
   * @todo Find out whether the default assignment operator can be used to simplify implementations of this method.
   */
  virtual void cloneDeepInto(DeepCopyable_shptr destination, oldnew_t *oldnew) const = 0;
};

/**
 * DeepCopyable is a type for cloneable objects (deep-copyable).
 *
 * You can think of an object hierarchy as a graph. To make a deep-copy possible,
 * all leafs of the graph must implement the DeepCopyable interface, and the inner nodes
 * must implement the DeepCopyableBase interface. Then, it is possible to recursively
 * visit all nodes in the graph by calling cloneDeep().
 *
 * The \a oldnew mapping is used to ensure that each object in the object hierarchy
 * is copied only once, even if there are multiple paths to the object in the
 * hierarchy. This is achieved by searching for an existing clone in the map
 * before cloning the respective object.
 */
class DeepCopyable : public DeepCopyableBase, public std::enable_shared_from_this<DeepCopyable> {
public:
  /**
   * @brief Return a copy of this object, but with all references to other DeepCopyables cleared (e.g. set to 0-pointer).
   */
  virtual DeepCopyable_shptr cloneShallow() const = 0;
  
  /**
   * @brief Return a deep-copy of this object, i.e. with all DeepCopyable members deep-copied themselves.
   * 
   * Use \a oldnew to avoid cloning objects multiple times.
   */
  virtual DeepCopyable_shptr cloneDeep(oldnew_t *oldnew) const;
protected:
  /**
   * @brief Store o->cloneShallow() in oldnew[o], unless there already exists such a clone.
   * 
   * This should be used as convenience function in cloneDeep() implementations.
   */
  static bool cloneOnce(const c_DeepCopyable_shptr &o, oldnew_t *oldnew);
};

}

#endif	/* DEEPCOPYABLE_H */

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

#ifndef __SINGLETONBASE_H__
#define __SINGLETONBASE_H__


#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>


namespace degate {

  /**
   * This is a base class for singletons.
   */

  template<class T>
  class SingletonBase : private boost::noncopyable {

  private:
    static boost::scoped_ptr<T> t;
    static boost::once_flag flag;

  protected:

    /**
     * Constructor.
     */

    SingletonBase() {}

    /**
     * Destructor.
     */

    virtual ~SingletonBase() {}

  public:

    /**
     * Get a reference to the singleton.
     */

    static T & get_instance() {
      boost::call_once(init, flag);
      return *t;
    }

    /**
     * Initialize the singleton.
     */

    static void init() {
      t.reset(new T());
    }

  };

  template<class T> boost::scoped_ptr<T> SingletonBase<T>::t(0);
  template<class T> boost::once_flag SingletonBase<T>::flag = BOOST_ONCE_INIT;


}

#endif

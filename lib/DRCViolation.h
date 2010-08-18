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

#ifndef __DRCVIOLATION_H__
#define __DRCVIOLATION_H__

#include <boost/foreach.hpp>
#include <tr1/memory>
#include <list>
#include <LogicModel.h>

namespace degate {

  class DRCViolation {
  private:

    PlacedLogicModelObject_shptr _obj;
    std::string _problem_description;
    std::string _drc_violation_class;

  public:
    DRCViolation(PlacedLogicModelObject_shptr obj,
		 std::string const& problem_description,
		 std::string const& drc_violation_class) :
      _obj(obj), 
      _problem_description(problem_description),
      _drc_violation_class(drc_violation_class) {
    }

    std::string get_problem_description() const {
      return _problem_description;
    }

    std::string get_drc_violation_class() const {
      return _drc_violation_class;
    }

    PlacedLogicModelObject_shptr get_object() const {
      return _obj;
    }
  };

  typedef std::tr1::shared_ptr<DRCViolation> DRCViolation_shptr;
}

#endif

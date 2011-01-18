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
#include <DRCBase.h>

namespace degate {

  class DRCViolation {
  private:

    PlacedLogicModelObject_shptr _obj;
    std::string _problem_description;
    std::string _drc_violation_class;
    DRC_SEVERITY _severity;

  public:

    /** Create a new Design Rule Check violation.
     * @param obj The object, which is affected from the violation.
     * @param problem_description A string, which contains a description
     *   of the violated contraint.
     * @param drc_violation_class This is a unique technical name for
     *   a DRC violation, that indicates the problem class.
     * @param severity Indicates the type of problem, actually
     *   if a DRC violation is just a warning or a real error.
     */
     
    DRCViolation(PlacedLogicModelObject_shptr obj,
		 std::string const& problem_description,
		 std::string const& drc_violation_class,
		 DRC_SEVERITY severity = DRC_ERROR) :
      _obj(obj),
      _problem_description(problem_description),
      _drc_violation_class(drc_violation_class),
      _severity(severity) {
    }

    std::string get_problem_description() const {
      return _problem_description;
    }

    std::string get_drc_violation_class() const {
      return _drc_violation_class;
    }

    DRC_SEVERITY get_severity() const {
      return _severity;
    }

    std::string get_severity_as_string() const {
      switch(_severity) {
      case DRC_ERROR: return "error"; break;
      case DRC_WARNING: return "warning"; break;      
      case DRC_UNDEFINED: 
      default: return "undefined"; break;
      }
    }

    static DRC_SEVERITY get_severity_from_string(std::string const & str) {
      if(str == "error") return DRC_ERROR;
      else if(str == "warning") return DRC_WARNING;
      return DRC_UNDEFINED;
    }

    PlacedLogicModelObject_shptr get_object() const {
      return _obj;
    }

    /**
     * Check if two DRC violations are conceptually equal.
     */
    bool equals(DRCViolation_shptr drcv) const {
      return 
	_obj == drcv->_obj &&
	_problem_description == drcv->_problem_description &&
	_drc_violation_class == drcv->_drc_violation_class &&
	_severity == drcv->_severity;
    }


  };

}

#endif

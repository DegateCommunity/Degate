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

#ifndef __RCVIOLATION_H__
#define __RCVIOLATION_H__

#include <boost/foreach.hpp>
#include <memory>
#include <list>
#include <Core/LogicModel/LogicModel.h>
#include <Core/RuleCheck/RCBase.h>

namespace degate
{
	class RCViolation
	{
	private:

		PlacedLogicModelObject_shptr obj;
		std::string                  problem_description;
		std::string                  rc_violation_class;
		RC_SEVERITY                  severity;

	public:

		/** Create a new Rule Check violation.
		 * @param obj The object, which is affected from the violation.
		 * @param problem_description A string, which contains a description
		 *   of the violated contraint.
		 * @param rc_violation_class This is a unique technical name for
		 *   a rc violation, that indicates the problem class.
		 * @param severity Indicates the type of problem, actually
		 *   if a rc violation is just a warning or a real error.
		 */
		RCViolation(PlacedLogicModelObject_shptr obj,
		            std::string const& problem_description,
		            std::string const& rc_violation_class,
		            RC_SEVERITY severity = RC_ERROR) :
                obj(obj),
                problem_description(problem_description),
                rc_violation_class(rc_violation_class),
                severity(severity)
		{
		}

		std::string get_problem_description() const
		{
			return problem_description;
		}

		std::string get_rc_violation_class() const
		{
			return rc_violation_class;
		}

		RC_SEVERITY get_severity() const
		{
			return severity;
		}

		std::string get_severity_as_string() const
		{
			switch (severity)
			{
			case RC_ERROR: return "error";
				break;
			case RC_WARNING: return "warning";
				break;
			case RC_UNDEFINED:
			default: return "undefined";
				break;
			}
		}

		static RC_SEVERITY get_severity_from_string(std::string const& str)
		{
			if (str == "error") return RC_ERROR;
			else if (str == "warning") return RC_WARNING;
			return RC_UNDEFINED;
		}

		bool matches_filter(std::string const& filter_pattern) const
		{
			return filter_pattern.empty() ||
				get_rc_violation_class().find(filter_pattern) != std::string::npos ||
				get_severity_as_string().find(filter_pattern) != std::string::npos ||
				get_problem_description().find(filter_pattern) != std::string::npos;
		}

		PlacedLogicModelObject_shptr get_object() const
		{
			return obj;
		}

		/**
		 * Check if two rc violations are conceptually equal.
		 */
		bool equals(RCViolation_shptr rcv) const
		{
			return
                    obj == rcv->obj &&
                    problem_description == rcv->problem_description &&
                    rc_violation_class == rcv->rc_violation_class &&
                    severity == rcv->severity;
		}
	};
}

#endif

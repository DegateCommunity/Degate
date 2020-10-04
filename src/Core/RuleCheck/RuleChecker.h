/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
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

#ifndef __RULECHECKER_H__
#define __RULECHECKER_H__

#include "Core/RuleCheck/RCBase.h"
#include "Core/RuleCheck/ERCRegister.h"

namespace degate
{
	class RuleChecker
	{
	private:

		std::list<RCBase_shptr> checks;
        RCVContainer rc_violations;

	public:

		RuleChecker()
		{
		    for (auto& e : ERC_REGISTER.get_erc_list())
            {
                checks.push_back(e);
            }
		}

		void run(LogicModel_shptr lmodel)
		{
			debug(TM, "run RC");

            rc_violations.clear();

			BOOST_FOREACH(RCBase_shptr check, checks)
			{
				check->run(lmodel);
				BOOST_FOREACH(RCViolation_shptr violation, check->get_rc_violations())
				{
                    rc_violations.push_back(violation);
				}
			}

			debug(TM, "found %d rc violations.", rc_violations.size());
		}

        /**
         * Get the list of RC violations.
         */
        RCVContainer get_rc_violations() const
        {
            return rc_violations;
        }
	};
}

#endif

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

#ifndef __ERCOPENPORTS_H__
#define __ERCOPENPORTS_H__

#include "Core/RuleCheck/RCBase.h"
#include "Core/LogicModel/LogicModel.h"

namespace degate
{
	/**
	 * Electrical Rule Checks that detects open ports, that means if
	 * a port is electrically unconnected.
	 */
	class ERCOpenPorts : public RCBase
	{
    Q_DECLARE_TR_FUNCTIONS(degate::ERCOpenPorts)

	public:

		ERCOpenPorts();

		void run(LogicModel_shptr lmodel) override;

        std::string generate_description(const RCViolation& violation) override;
	};
}

#endif

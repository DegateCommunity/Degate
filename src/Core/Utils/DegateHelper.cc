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

#include "Core/Utils/DegateHelper.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace degate;
using namespace std;

std::vector<std::string> degate::tokenize(std::string const& str)
{
	/* This implementation is more or less directly derived from
	   this posting http://www.gamedev.net/community/forums/topic.asp?topic_id=320087

	*/
	vector<string> result;

	string item;
	stringstream ss(str);

	while (ss >> item)
	{
		if (item[0] == '"')
		{
			int last_item_position = item.length() - 1;
			if (item[last_item_position] != '"')
			{
				// read the rest of the double-quoted item
				string rest_of_item;
				getline(ss, rest_of_item, '"');
				item += rest_of_item;
			}
			// otherwise, we had a single word that was quoted. In any case, we now
			// have the item in quotes; remove them
			item = item.substr(1, last_item_position - 1);
		}
		// item is "fully cooked" now
		result.push_back(item);
	}
	return result;
}

void degate::write_string_to_file(std::string const& path,
                                  std::string const& content)
{
	std::ofstream file;
	file.open(path.c_str(), std::ios::trunc | std::ios::out);
	file << content;
	file.close();
}

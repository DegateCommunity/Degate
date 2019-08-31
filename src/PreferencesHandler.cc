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

#include "PreferencesHandler.h"

namespace degate
{
	PreferencesHandler::PreferencesHandler()
	{
		ret_t res = check_file(DEGATE_CONFIG_FILE_PATH);

		if(res != RET_OK)
			return;

		QDomDocument parser;

		QFile file(QString::fromStdString(DEGATE_CONFIG_FILE_PATH));
		if (!file.open(QIODevice::ReadOnly))
		{
			debug(TM, "Problem: can't open the file %s.", DEGATE_CONFIG_FILE_PATH);
			return;
		}

		if (!parser.setContent(&file))
		{
			debug(TM, "Problem: can't parse the file %s.", DEGATE_CONFIG_FILE_PATH);
			return;
		}
		file.close();

		const QDomElement root_elem = parser.documentElement();
		assert(!root_elem.isNull());

		// Theme
		QDomElement theme_element = get_dom_twig(root_elem, "theme");
		QString theme = theme_element.attribute("theme");
		this->theme = string_to_theme(theme.toStdString());

		// Icon Theme
		QString icon_theme = theme_element.attribute("icon_theme");
		this->icon_theme = string_to_icon_theme(icon_theme.toStdString());
	}

	PreferencesHandler::~PreferencesHandler()
	{
		save();
	}

	void PreferencesHandler::save()
	{
		QDomDocument doc;

		QDomElement root_elem = doc.createElement("config");
		assert(!root_elem.isNull());

		QDomElement theme_elem = doc.createElement("theme");
		
		// Theme
		theme_elem.setAttribute("theme", QString::fromStdString(theme_to_string(this->theme)));

		// Icon Theme
		theme_elem.setAttribute("icon_theme", QString::fromStdString(icon_theme_to_string(this->icon_theme)));

		root_elem.appendChild(theme_elem);
		
		doc.appendChild(root_elem);

		QFile file(QString::fromStdString(DEGATE_CONFIG_FILE_PATH));
		if (!file.open(QIODevice::WriteOnly))
		{
			throw InvalidPathException("Can't create export file.");
		}

		QTextStream stream(&file);
		stream << doc.toString();

		file.close();
	}

	Theme PreferencesHandler::get_theme()
	{
		return theme;
	}

	void PreferencesHandler::set_theme(Theme theme)
	{
		this->theme = theme;
	}

	IconTheme PreferencesHandler::get_icon_theme()
	{
		return this->icon_theme;
	}

	void PreferencesHandler::set_icon_theme(IconTheme theme)
	{
		this->icon_theme = theme;
	}
}

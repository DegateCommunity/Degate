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

#ifndef __COLORSELECTION_H__
#define __COLORSELECTION_H__

#include "MemoryMap.h"

#include <QPushButton>
#include <QColorDialog>
#include <QColor>

namespace degate
{
	class ColorSelectionButton : public QPushButton
	{
	    Q_OBJECT

	public:
		ColorSelectionButton(QWidget* parent, const QString& name = QString());
		~ColorSelectionButton();

	    void set_color(const color_t& color);
	    color_t get_color();

	public slots:
	    void update_color();

	private:
	    color_t color;
	};
}

#endif
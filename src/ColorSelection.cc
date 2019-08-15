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

#include "ColorSelection.h"
#include "Image.h"

namespace degate
{
	ColorSelectionButton::ColorSelectionButton(QWidget* parent, const QString& name) : QPushButton(parent)
	{
		setText(name);

		QObject::connect(this, SIGNAL(clicked()), this, SLOT(update_color()));
	}

	ColorSelectionButton::~ColorSelectionButton()
	{
	}

	void ColorSelectionButton::set_color(const color_t& color)
	{
		this->color = color;
		setStyleSheet("background-color: rgba(" + QString::number(MASK_R(color)) + ", " + QString::number(MASK_G(color)) + ", " + QString::number(MASK_B(color)) + ", " + QString::number(MASK_A(color)) + ")");
	}

	color_t ColorSelectionButton::get_color()
	{
		return color;
	}

	void ColorSelectionButton::update_color()
	{
		QRgb new_color = QColorDialog::getRgba(qRgba(MASK_R(color), MASK_G(color), MASK_B(color), MASK_A(color)), NULL, parentWidget());

		color = MERGE_CHANNELS(qRed(new_color), qGreen(new_color), qBlue(new_color), qAlpha(new_color));
		setStyleSheet("background-color: rgba(" + QString::number(MASK_R(color)) + ", " + QString::number(MASK_G(color)) + ", " + QString::number(MASK_B(color)) + ", " + QString::number(MASK_A(color)) + ")");
	}
}

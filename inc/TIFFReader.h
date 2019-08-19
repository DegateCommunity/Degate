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

#ifndef __TIFFREADER_H__
#define __TIFFREADER_H__

#include <list>
#include <memory>
#include <QImageReader>

//#include "ImageReaderFactory.h"
#include "StoragePolicies.h"
#include "ImageReaderBase.h"
#include "Image.h"

namespace degate
{
	/**
	 * The TIFFReader parses tiff images.
	 */

	template <class ImageType>
	class TIFFReader : public ImageReaderBase<ImageType>
	{
	private:

		QImage* image = NULL;


	public:

		using ImageReaderBase<ImageType>::get_filename;
		using ImageReaderBase<ImageType>::set_width;
		using ImageReaderBase<ImageType>::set_height;
		using ImageReaderBase<ImageType>::get_width;
		using ImageReaderBase<ImageType>::get_height;

		TIFFReader(std::string const& filename) :
			ImageReaderBase<ImageType>(filename),
			image(NULL)
		{
		}

		~TIFFReader()
		{
			if (image != NULL) delete image;
		}

		bool read();

		bool get_image(std::shared_ptr<ImageType>);
	};

	template <class ImageType>
	bool TIFFReader<ImageType>::read()
	{
		QImageReader reader(get_filename().c_str());
		QSize size = reader.size();
		if (!size.isValid())
		{
			debug(TM, "can't read size of %s\n", get_filename().c_str());
			return false;
		}

		set_width(size.width());
		set_height(size.height());

		//assert(reader.imageFormat() == QImage::Format_ARGB32 || reader.imageFormat() == QImage::Format_RGB32);

		image = new QImage(size.width(), size.height(), reader.imageFormat());

		debug(TM, "Reading image with size: %d x %d", get_width(), get_height());

		if (!reader.read(image))
		{
			debug(TM, "can't read %s\n", get_filename().c_str());
			return false;
		}

		debug(TM, "Image read\n");

		return true;
	}

	template <class ImageType>
	bool TIFFReader<ImageType>::get_image(std::shared_ptr<ImageType> img)
	{
		if (image == NULL) return false;
		if (img == NULL) return false;

		for (unsigned int y = 0; y < get_height(); y++)
		{
			for (unsigned int x = 0; x < get_width(); x++)
			{
				QRgb rgb = image->pixel(x, y);
				img->set_pixel(x, y, MERGE_CHANNELS(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb)));
			}
		}

		return true;
	}
}

#endif

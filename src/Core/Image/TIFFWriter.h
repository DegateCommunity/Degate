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

#ifndef __TIFFWRITER_H__
#define __TIFFWRITER_H__

#include <list>
#include <memory>
#include <QImageWriter>

#include <Globals.h>
#include <Core/Utils/DegateExceptions.h>
#include <Core/Image/PixelPolicies.h>
#include <Core/Image/StoragePolicies.h>
#include <Core/Image/ImageWriterBase.h>
#include <Core/Utils/FileSystem.h>
#include <Core/Image/Image.h>

#include <cerrno>

namespace degate
{
	/**
	 * The TIFFWriter parses tiff images.
	 */
	template <class ImageType>
	class TIFFWriter : public ImageWriterBase<ImageType>
	{
	public:

		using ImageWriterBase<ImageType>::get_filename;
		using ImageWriterBase<ImageType>::get_width;
		using ImageWriterBase<ImageType>::get_height;


		TIFFWriter(unsigned int width, unsigned int height,
		           std::string const& filename) :
			ImageWriterBase<ImageType>(width, height, filename)
		{
		}

		virtual ~TIFFWriter()
		{
		}

		/**
		 * exception FileSystemException
		 */
		bool write_image(std::shared_ptr<ImageType> img);
	};


	template <class ImageType>
	bool TIFFWriter<ImageType>::write_image(std::shared_ptr<ImageType> img)
	{
		QImageWriter writer(get_filename().c_str());

		// Write the tiff tags to the file
		writer.setFormat("TIFF");
		writer.setCompression(0);
		writer.setText("ImageWidth", QString::number(get_width()));
		writer.setText("ImageLength", QString::number(get_height()));
		writer.setText("Compression", "1");
		writer.setText("PlanarConfiguration", "1");
		writer.setText("PhotometricInterpretation", "2");
		writer.setText("BitsPerSample", "8");
		writer.setText("SamplesPerPixel", "3");

		QImage image(get_width(), get_height(), QImage::Format_ARGB32);

		for (unsigned int y = 0; y < get_height(); y++)
		{
			for (unsigned int x = 0; x < get_width(); x++)
			{
				rgba_pixel_t p = img->template get_pixel_as<rgba_pixel_t>(x, y);

				image.setPixel(x, y, qRgba(MASK_R(p), MASK_G(p), MASK_B(p), MASK_A(p)));
			}
		}

		if (!writer.write(image))
		{
			return false;
		}

		return true;
	}
}

#endif

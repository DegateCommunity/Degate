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

#ifndef __IMAGEREADERFACTORY_H__
#define __IMAGEREADERFACTORY_H__

#include <list>
#include <tr1/memory>
#include <degate_exceptions.h>

#include <StoragePolicies.h>
#include <PixelPolicies.h>
#include <FileSystem.h>
#include <ImageReaderBase.h>
#include <TIFFReader.h>
#include <JPEGReader.h>


namespace degate{

  typedef std::list<std::string> file_format_collection;

  /**
   * A factory for creating image reader objects.
   */

  template<class ImageType>
  class ImageReaderFactory {

  public:

  private:

    file_format_collection file_formats;

  public:

    ImageReaderFactory() {
      // Tiff file format parser
      file_formats.push_back("tif");
      file_formats.push_back("tiff");

      // Jpeg file format parser
      file_formats.push_back("jpg");
      file_formats.push_back("jpeg");
    }


    ~ImageReaderFactory() {}

    /**
     * Get a list of parseable file formats.
     */
    file_format_collection const& get_file_formats() const {
      return file_formats;
    }

    /**
     * Get a concrete image reader object for a file name.
     * @param filename The file name is evaluated. Depending on the file suffix an
     *   image reader is choosen. The file must not exists for the reader selection,
     *   but the reader will fail, if it can't read the image.
     * @return Returns an image reader object that is able to read the image.
     * @exception InvalidFileFormatException This exception is thrown if the there is no
     *   reader that can read the file.
     */

    std::tr1::shared_ptr<class ImageReaderBase<ImageType> >
    get_reader(std::string const & filename) {

      std::string suffix(get_file_suffix(filename).c_str());

      /*
	Convert suffix string to a lowercase string.

	This comment should not be necessary, if there would be a simple
	way to convert it. You may have seen other programming languages
	with a simple convenient way to lowercase strings. But hey! The
	transform is really generic, You can even apply it to strings. Wow!
	BTW: If you omit the static cast, you will get a long error message
	from your compiler. Great!
      */

      std::transform(suffix.begin(), suffix.end(), suffix.begin(),
		     static_cast<int (*)(int)>(std::tolower));

      // Ok. Enough sarcasm. Let us finish the job ...

      if(suffix == "tif" || suffix == "tiff")
	return std::tr1::shared_ptr<ImageReaderBase<ImageType> >
	  (new TIFFReader<ImageType>(filename));
      else if(suffix == "jpg" || suffix == "jpeg")
	return std::tr1::shared_ptr<ImageReaderBase<ImageType> >
	  (new JPEGReader<ImageType>(filename));
      else throw InvalidFileFormatException();
    }


  };

}




#endif

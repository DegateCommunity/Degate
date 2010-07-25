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

#ifndef __IMAGE_H__
#define __IMAGE_H__


#ifdef WORDS_BIGENDIAN
#define MASK_R(r) (r >> 24)
#define MASK_G(g) ((g >> 16) & 0xff)
#define MASK_B(b) ((b >> 8) & 0xff)
#define MASK_A(a) (a & 0xff)
#define MERGE_CHANNELS(r,g,b,a) ((r << 24) | (g <<16) | (b << 8) | a)
#else
#define MASK_R(r) (r & 0xff)
#define MASK_G(g) ((g >> 8) & 0xff)
#define MASK_B(b) ((b >> 16) & 0xff)
#define MASK_A(a) (a >> 24)
#define MERGE_CHANNELS(r,g,b,a) ((a << 24) | (b <<16) | (g << 8) | r)
#endif

// 75 * 255 + 147 * 255 + 35 * 255 = 65535
#define RGBA_TO_GS_BY_PTR(pix_ptr) RGBA_TO_GS_BY_VAL(*pix_ptr)
#define RGBA_TO_GS_BY_VAL(pix) (((75 * MASK_R(pix)) + (147 * MASK_G(pix)) + (35 * MASK_B(pix))) >> 8)


#include "globals.h"
#include "PixelPolicies.h"
#include "StoragePolicies.h"
#include "TileImage.h"
#include "FileSystem.h"
#include "Configuration.h"
#include "TypeTraits.h"
#include "TypeConstraints.h"
#include "ImageManipulation.h"


namespace degate {

  class ImageBase {

  private:

    BoundingBox bounding_box;

  public:

    ImageBase(unsigned int _width, unsigned int _height) :
      bounding_box(_width, _height) {}

    virtual ~ImageBase() {}

    /**
     * Get the width of an image.
     */

    inline unsigned int get_width() const { 
      return bounding_box.get_width(); 
    }
    
    /**
     * Get the height of an image.
     */

    inline unsigned int get_height() const { 
      return bounding_box.get_height(); 
    }

    /**
     * Get the bounding box that represents the image.
     */
    inline BoundingBox const& get_bounding_box() const {
      return bounding_box;
    }

  };


  /**
   * The generic templated class for image objects.
   *
   * This class can be used with StoragePolicy_Memory and
   * StoragePolicy_TempFile.
   *
   * Every image constructor should have a two parameter
   * version. This is necessary for image processing plugins,
   * because plugins have to create temporary images. The two
   * parameter constructor should then make reasonable default
   * assumtions about storage. Temporary images are really always
   * temporary. This means that we can create empty images in
   * degate's temp directory. The constructor should only get
   * the width and height of an image.
   */

  template<class PixelPolicy, 
	   template <class PixelPolicy> class StoragePolicy>
  class Image : public ImageBase,
		public StoragePolicy<PixelPolicy> {
    
  public:

    /**
     * The constructor.
     */
    
    Image(unsigned int _width, unsigned int _height) :
      ImageBase(_width, _height),
      StoragePolicy<PixelPolicy>(_width, _height) {}

    /**
     * The destructor.
     */
    virtual ~Image() {}    


    /**
     * Get pixel with conversion.
     */
    template<typename PixelTypeDst>
    inline PixelTypeDst get_pixel_as(unsigned int x, unsigned int y) {
      return convert_pixel<PixelTypeDst, typename PixelPolicy::pixel_type>(this->get_pixel(x, y));
    }

    /**
     * Set pixel with conversion.
     */
    template<typename PixelTypeSrc>
    inline void set_pixel_as(unsigned int x, unsigned int y, PixelTypeSrc p) {
      this->set_pixel(x, y, 
		      convert_pixel<typename PixelPolicy::pixel_type, PixelTypeSrc>(p));
    }

  };
  
  typedef std::tr1::shared_ptr<ImageBase> ImageBase_shptr;

  /**
   * Partial template specialization for the storage policy StoragePolicy_PersistentFile.
   */

  template<class PixelPolicy>
  class Image<PixelPolicy, StoragePolicy_PersistentFile> : 
    public ImageBase,
    public StoragePolicy_PersistentFile<PixelPolicy> {

  public:

    /**
     * @todo The third parameter must be optional. Create a temp image instead.
     */
    Image(unsigned int _width, 
	  unsigned int _height, 
	  std::string const& filename) : 
      ImageBase(_width, _height),
      StoragePolicy_PersistentFile<PixelPolicy>(_width, 
						_height, 
						filename) {}

    virtual ~Image() {}


    /**
     * Get pixel with conversion.
     */
    template<typename PixelTypeDst>
    inline PixelTypeDst get_pixel_as(unsigned int x, unsigned int y) {
      return convert_pixel<PixelTypeDst, typename PixelPolicy::pixel_type>(this->get_pixel(x, y));
    }

    /**
     * Set pixel with conversion.
     */
    template<typename PixelTypeSrc>
    inline void set_pixel_as(unsigned int x, unsigned int y, PixelTypeSrc p) {
      this->set_pixel(x, y, 
		      convert_pixel<typename PixelPolicy::pixel_type, PixelTypeSrc>(p));
    }


  };


  /**
   * Partial template specialization for the storage policy StoragePolicy_Tile.
   */

  template<class PixelPolicy>
  class Image<PixelPolicy, StoragePolicy_Tile> : 
    public ImageBase,
    public StoragePolicy_Tile<PixelPolicy> {

  public:

    /**
     * Constructor for temporary virtual images.
     */

    Image(unsigned int _width, 
	  unsigned int _height,
	  unsigned int _tile_width_exp = 10) : 
      ImageBase(_width, _height),
      StoragePolicy_Tile<PixelPolicy>(_width, _height,
				      create_temp_directory(),
				      false,
				      _tile_width_exp) {}
    
    /**
     * Constructor for persistent virtual images.
     */

    Image(unsigned int _width, 
	  unsigned int _height,
	  std::string const& directory,
	  bool persistent = true,
	  unsigned int _tile_width_exp = 10) : 
      ImageBase(_width, _height),
      StoragePolicy_Tile<PixelPolicy>(_width, _height, 
				      directory,
				      persistent,
				      _tile_width_exp) {}

    /**
     * The dtor.
     */

    virtual ~Image() {}

    /**
     * Get pixel with conversion.
     */
    template<typename PixelTypeDst>
    inline PixelTypeDst get_pixel_as(unsigned int x, unsigned int y) {
      return convert_pixel<PixelTypeDst, typename PixelPolicy::pixel_type>(this->get_pixel(x, y));
    }

    /**
     * Set pixel with conversion.
     */
    template<typename PixelTypeSrc>
    inline void set_pixel_as(unsigned int x, unsigned int y, PixelTypeSrc p) {
      this->set_pixel(x, y, 
		      convert_pixel<typename PixelPolicy::pixel_type, PixelTypeSrc>(p));
    }
  };

 
  /**
   * Typedefs for common types of virtual images.
   */


  typedef Image<PixelPolicy_RGBA, StoragePolicy_Tile> TileImage_RGBA;
  typedef Image<PixelPolicy_GS_DOUBLE, StoragePolicy_Tile> TileImage_GS_DOUBLE;
  typedef Image<PixelPolicy_GS_BYTE, StoragePolicy_Tile> TileImage_GS_BYTE;

  typedef std::tr1::shared_ptr<TileImage_RGBA> TileImage_RGBA_shptr;
  typedef std::tr1::shared_ptr<TileImage_GS_DOUBLE> TileImage_GS_DOUBLE_shptr;
  typedef std::tr1::shared_ptr<TileImage_GS_BYTE> TileImage_GS_BYTE_shptr;


  typedef Image<PixelPolicy_RGBA, StoragePolicy_Tile> BackgroundImage;
  typedef std::tr1::shared_ptr<BackgroundImage> BackgroundImage_shptr;


  typedef Image<PixelPolicy_RGBA, StoragePolicy_TempFile> TempImage_RGBA;
  typedef Image<PixelPolicy_GS_DOUBLE, StoragePolicy_TempFile> TempImage_GS_DOUBLE;
  typedef Image<PixelPolicy_GS_BYTE, StoragePolicy_TempFile> TempImage_GS_BYTE;

  typedef std::tr1::shared_ptr<TempImage_RGBA> TempImage_RGBA_shptr;
  typedef std::tr1::shared_ptr<TempImage_GS_DOUBLE> TempImage_GS_DOUBLE_shptr;
  typedef std::tr1::shared_ptr<TempImage_GS_BYTE> TempImage_GS_BYTE_shptr;
  

  typedef Image<PixelPolicy_RGBA, StoragePolicy_PersistentFile> PersistentImage_RGBA;
  typedef std::tr1::shared_ptr<PersistentImage_RGBA> PersistentImage_RGBA_shptr;

  typedef Image<PixelPolicy_RGBA, StoragePolicy_Memory> RendererImage;
  typedef std::tr1::shared_ptr<RendererImage> RendererImage_shptr;


  // typedef for the image format in which we store template images within memory
  typedef Image<PixelPolicy_RGBA, StoragePolicy_Memory> GateTemplateImage;
  typedef std::tr1::shared_ptr<GateTemplateImage> GateTemplateImage_shptr;


}

#endif


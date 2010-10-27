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

#ifndef __DEGATE_EXCEPTIONS_H__
#define __DEGATE_EXCEPTIONS_H__

#include <stdexcept>

namespace degate {

  /**
   * Generic runtime exception in libdegate.
   * @todo Check for std::runtime_error exceptions in the
   *   code and replace it with a DegateRuntimeException.
   */

  class DegateRuntimeException : public std::runtime_error {
  public:
    DegateRuntimeException() : std::runtime_error("There is a generic runtime error in libdegate.") {}
    DegateRuntimeException(std::string str) : std::runtime_error(str) {}
  };


  /**
   * Represents a this-should-not-happen problem and indicates a
   * programming error. This class is is derived from std::logic_error.
   * This excpetion should not go unnoticed. You should not
   * catch it. It is ok, that the application will abort, because
   * this exception already indicates that it is already broken.
   *
   * @todo Check for std::logic_error exceptions in the
   *   code and replace it with a GenericDegateException.
   */

  class DegateLogicException : public std::logic_error {
  public:
    DegateLogicException() : 
      std::logic_error("There is a generic logic error in libdegate. "
		       "This indicates a programming error.") {}
    DegateLogicException(std::string str) : std::logic_error(str) {}
  };
  

  /**
   * This exception is raised if a code fragment receives a null pointer
   * where this should not happen.
   */

  class InvalidPointerException : public DegateLogicException {
  public:
    InvalidPointerException() : DegateLogicException("Invalid pointer value.") {}
    InvalidPointerException(std::string str) : DegateLogicException(str) {}
  };


  /**
   * This exception is raised if a method detects an invalid Object ID.
   * Object IDs are managed by the logic model. Probably the object was
   * not inserted into the logic model.
   */

  class InvalidObjectIDException : public DegateLogicException {
  public:
    InvalidObjectIDException() : 
      DegateLogicException("The logic model object has no or an invalid object ID.") {}
    InvalidObjectIDException(std::string str) : DegateLogicException(str) {}
  };


  /**
   * Indicates a runtime exception from the XML parser.
   */
  
  class InvalidXMLException : public DegateRuntimeException {
  public:
    InvalidXMLException() : DegateRuntimeException("XML is invalid." ) {}
    InvalidXMLException(std::string const & str) : DegateRuntimeException(str) {}
  };
  
  /**
   * This exception is thrown if a XML importer failed to parse a
   * value of an XML attribute.
   */

  class XMLAttributeParseException : public DegateRuntimeException {
  public:
    XMLAttributeParseException() : DegateRuntimeException("Can't parse XML attribute value." ) {}
    XMLAttributeParseException(std::string const & str) : DegateRuntimeException(str) {}
  };

  /**
   * This exception is thrown if a XML importer detects a missing XML attribute
   * and this XML attribute must be present.
   */

  class XMLAttributeMissingException : public DegateRuntimeException {
  public:
    XMLAttributeMissingException() : DegateRuntimeException("XML attribute is missing." ) {}
    XMLAttributeMissingException(std::string const & str) : DegateRuntimeException(str) {}
  };

  /**
   * This exception is thrown if a method failed to lookup an object
   * in a collection type, such as in a std::map or list.
   */
  
  class CollectionLookupException : public DegateRuntimeException {
  public:
    CollectionLookupException() : DegateRuntimeException("Failed to lookup an object in a collection." ) {}
    CollectionLookupException(std::string const & str) : DegateRuntimeException(str) {}
  };

  /**
   * This exception indicates, that a file system operation failed.
   */

  class FileSystemException : public DegateRuntimeException {
  public:
    FileSystemException() : DegateRuntimeException("The file doesn't exist." ) {}
    FileSystemException(std::string const & str) : DegateRuntimeException(str) {}
  };
  
  
  /**
   * This exception indicates an invalid path in the file system.
   */
  class InvalidPathException : public FileSystemException {
  public:
    InvalidPathException() : FileSystemException("The file doesn't exist." ) {}
    InvalidPathException(std::string const & str) : FileSystemException(str) {}
  };

  /**
   * This exception indicates, that libdegate is unable to handle the image file format.
   */
  class InvalidFileFormatException : public DegateRuntimeException {
  public:
    InvalidFileFormatException() : 
      DegateRuntimeException("Can't read the file. It is an unknown image format.") {}
    InvalidFileFormatException(std::string const & str) : DegateRuntimeException(str) {}
  };


  /**
   * This exception is thrown if a ZIP archive cannot be exported.
   */

  class ZipException : public DegateRuntimeException {
  public:
    ZipException() : DegateRuntimeException("Can't export ZIP archive." ) {}
    ZipException(std::string const & str) : DegateRuntimeException(str) {}
  };

  /**
   * Indicates a runtime exception from the XML parser.
   */
  
  class XMLRPCException : public DegateRuntimeException {
  public:
    XMLRPCException() : DegateRuntimeException("XMLRPC failed." ) {}
    XMLRPCException(std::string const & str) : DegateRuntimeException(str) {}
  };
  

}

#endif

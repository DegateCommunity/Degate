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

#ifndef __EXTERNALMATCHING_H__
#define __EXTERNALMATCHING_H__

#include <Image.h>
#include <Project.h>
#include <TemplateMatching.h>

namespace degate {

  /**
   * Run an external program, that analyzes images.
   * The program extracts wires and vias and writes
   * the results into a file. The file is parsed
   * bu this class and the object are added into
   * the logic model.
   *
   * Here is a brief decription of the result file:
   *
   * Comment lines are starting with a hash mark ('#')
   *
   * Wire:
   *
   * Format: "wire" x1 y1 x2 y2 diameter
   * Example: wire 10 23 100 23 5
   *
   * Via:
   *
   * Format: "via" x y diameter direction
   * Example: via 42 23 5 up
   *
   * The direction is either "up" or "down"
   *
   * Strings are case sensitive.
   */
  class ExternalMatching : public Matching {

  private:

    Layer_shptr layer;
    LogicModel_shptr lmodel;
    BackgroundImage_shptr img;
    BoundingBox bounding_box;

    std::string cmd;
    int exit_code;

  private:

    std::list<PlacedLogicModelObject_shptr> parse_file(std::string const& filename) const;

    /**
     * Parse a line, that describes an object.
     * @exception DegateRuntimeExcpetion This expetion is thrown
     *   if a line cannot be parsed.
     * @return Returns a shared pointer to an object, that can be added
     *   to the logic model. It returns a NULL pointer equivalent, if the
     *   line does not represent a command.
     */

    PlacedLogicModelObject_shptr parse_line(std::string const& line) const;


  public:

    ExternalMatching();

    /**
     *
     * @exception InvalidPointerException
     * @exception DegateRuntimeException
     */
    virtual void init(BoundingBox const& bounding_box, Project_shptr project);

    virtual void run();

    void set_command(std::string const& cmd);
    std::string get_command() const;

    int get_exit_code() const;
  };

  typedef std::shared_ptr<ExternalMatching> ExternalMatching_shptr;
}

#endif

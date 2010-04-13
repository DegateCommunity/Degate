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

#ifndef __VHDLTBCODETEMPLATEGENERATOR_H__
#define __VHDLTBCODETEMPLATEGENERATOR_H__

#include <degate.h>
#include <tr1/memory>
#include <cctype>

#include <VHDLCodeTemplateGenerator.h>
#include <boost/foreach.hpp>

namespace degate {

  /**
   * A code template generator for VHDL.
   */
  
  class VHDLTBCodeTemplateGenerator : public VHDLCodeTemplateGenerator {
  public:
    
    VHDLTBCodeTemplateGenerator(std::string const& entity_name,
				std::string const& description,
				std::string const& logic_class);

    virtual ~VHDLTBCodeTemplateGenerator();
    virtual std::string generate() const;

  protected:

    //using VHDLCodeTemplateGenerator::generate_component;

    virtual std::string generate_header() const;

    virtual std::string generate_impl(std::string const& logic_class) const;

    std::string generate_signals() const;

    std::string generate_clock_process(std::string const& clock_signal_name) const;


  };

  typedef std::tr1::shared_ptr<VHDLTBCodeTemplateGenerator> VHDLTBCodeTemplateGenerator_shptr;

}

#endif

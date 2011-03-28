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

#ifndef __VERILOGMODULEGENERATOR_H__
#define __VERILOGMODULEGENERATOR_H__

#include <degate.h>
#include <tr1/memory>

#include <VerilogCodeTemplateGenerator.h>

namespace degate {

  class VerilogModuleGenerator : public VerilogCodeTemplateGenerator {
    
  private:

    Module_shptr mod;
    bool no_gates;

  public:
    
    VerilogModuleGenerator(Module_shptr module, bool do_not_output_gates = false);
    
    virtual ~VerilogModuleGenerator();
    
  protected:
    
    virtual std::string generate_common() const;

    virtual std::string generate_impl(std::string const& logic_class = ""  /* unused parameter */) const;

  private:

    std::string generate_common_code_for_gates(Module_shptr module, std::set<GateTemplate_shptr> & already_dumped) const;
  };


}

#endif

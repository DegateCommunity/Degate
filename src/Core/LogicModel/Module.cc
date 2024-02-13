/**
 * This file is part of the IC reverse engineering tool Degate.
 *
 * Copyright 2008, 2009, 2010 by Martin Schobert
 * Copyright 2012 Robert Nitsch
 * Copyright 2019-2020 Dorian Bachelot
 *
 * Degate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Degate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with degate. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Core/LogicModel/Module.h"
#include <boost/algorithm/string.hpp>

#include <iterator>

using namespace degate;


Module::Module(std::string const& module_name,
               std::string const& entity_name,
               bool is_root) :
    entity_name(entity_name),
    is_root(is_root)
{
    set_name(module_name);
}

Module::~Module()
{
}

DeepCopyable_shptr Module::clone_shallow() const
{
    auto clone = std::make_shared<Module>(get_name(), entity_name, is_root);
    return clone;
}

void Module::clone_deep_into(DeepCopyable_shptr dest, oldnew_t* oldnew) const
{
    auto clone = std::dynamic_pointer_cast<Module>(dest);

    // modules
    std::transform(modules.begin(), modules.end(), std::inserter(clone->modules, clone->modules.begin()),
                   [&](const module_collection::value_type& v)
                   {
                       return std::dynamic_pointer_cast<Module>(v->clone_deep(oldnew));
                   });

    // gates
    std::transform(gates.begin(), gates.end(), std::inserter(clone->gates, clone->gates.begin()),
                   [&](const gate_collection::value_type& v)
                   {
                       return std::dynamic_pointer_cast<Gate>(v->clone_deep(oldnew));
                   });

    // ports
    std::for_each(ports.begin(), ports.end(), [&](const port_collection::value_type& v)
    {
        clone->ports[v.first] = std::dynamic_pointer_cast<GatePort>(v.second->clone_deep(oldnew));
    });

    LogicModelObjectBase::clone_deep_into(dest, oldnew);
}

bool Module::is_main_module() const
{
    return is_root;
}

void Module::set_main_module()
{
    is_root = true;
}


void Module::set_entity_name(std::string const& name)
{
    entity_name = name;
}

std::string Module::get_entity_name() const
{
    return entity_name;
}


void Module::add_gate(Gate_shptr gate, bool detect_ports)
{
    if (gate == nullptr)
        throw InvalidPointerException("Invalid pointer passed to add_gate().");

    gates.insert(gate);
    if (!is_root && detect_ports) determine_module_ports();
}

bool Module::remove_gate(Gate_shptr gate)
{
    if (gate == nullptr)
        throw InvalidPointerException("Invalid pointer passed to remove_gate().");


    gate_collection::const_iterator g_iter = gates.find(gate);
    if (g_iter != gates.end())
    {
        gates.erase(g_iter);
        if (!is_root) determine_module_ports();
        return true;
    }
    else
    {
        for (module_collection::iterator iter = modules.begin();
             iter != modules.end(); ++iter)
        {
            Module_shptr child = *iter;
            if ((*iter)->remove_gate(gate) == true) return true;
        }
    }

    return false;
}


void Module::add_module(Module_shptr module)
{
    if (module == nullptr)
        throw InvalidPointerException("Invalid pointer passed to add_modue().");

    modules.push_back(module);
}


bool Module::remove_module(Module_shptr module)
{
    if (module == nullptr)
        throw InvalidPointerException("Invalid pointer passed to remove_module().");

    for (module_collection::iterator iter = modules.begin();
         iter != modules.end(); ++iter)
    {
        Module_shptr child = *iter;

        if (child == module)
        {
            child->move_gates_recursive(this);
            modules.erase(iter);
            return true;
        }
        else if ((*iter)->remove_module(module) == true)
            return true;
    }

    return false;
}

void Module::remove_port(std::string module_port_name)
{
    ports.erase(module_port_name);
}

void Module::move_gates_recursive(Module* dst_mod)
{
    if (dst_mod == nullptr)
        throw InvalidPointerException("Invalid pointer passed to move_all_child_gates_into_current_module().");

    for (gate_collection::iterator g_iter = gates_begin();
         g_iter != gates_end(); ++g_iter)
    {
        std::cout << "Add gate " << (*g_iter)->get_name() << " to module " << dst_mod->get_name() << std::endl;

        dst_mod->add_gate(*g_iter);
    }

    for (module_collection::iterator iter = modules.begin();
         iter != modules.end(); ++iter)
        (*iter)->move_gates_recursive(dst_mod);
}

Module::module_collection::iterator Module::modules_begin()
{
    return modules.begin();
}

Module::module_collection::iterator Module::modules_end()
{
    return modules.end();
}

Module::gate_collection::iterator Module::gates_begin()
{
    return gates.begin();
}

Module::gate_collection::iterator Module::gates_end()
{
    return gates.end();
}

Module::port_collection::iterator Module::ports_begin()
{
    return ports.begin();
}

Module::port_collection::iterator Module::ports_end()
{
    return ports.end();
}

Module::module_collection::const_iterator Module::modules_begin() const
{
    return modules.begin();
}

Module::module_collection::const_iterator Module::modules_end() const
{
    return modules.end();
}

Module::gate_collection::const_iterator Module::gates_begin() const
{
    return gates.begin();
}

Module::gate_collection::const_iterator Module::gates_end() const
{
    return gates.end();
}

Module::port_collection::const_iterator Module::ports_begin() const
{
    return ports.begin();
}

Module::port_collection::const_iterator Module::ports_end() const
{
    return ports.end();
}


void Module::automove_gates()
{
    /*
      from top to down in the module hierarchy:
  
      for each module: iterate over gates:
  
        iterate over gate ports:
        - port unconnected - ignore
        - port connected - check all related
  
     */
}


bool Module::net_completely_internal(Net_shptr net) const
{
    for (Net::connection_iterator c_iter = net->begin(); c_iter != net->end(); ++c_iter)
    {
        object_id_t oid = *c_iter;
        GatePort_shptr gport = lookup_gate_port_recursive(oid);
        if (gport == nullptr)
        {
            // external entity
            return false;
        }
    }
    return true;
}

bool Module::net_feeded_internally(Net_shptr net) const
{
    for (Net::connection_iterator c_iter = net->begin(); c_iter != net->end(); ++c_iter)
    {
        object_id_t oid = *c_iter;
        GatePort_shptr gport = lookup_gate_port_recursive(oid);

        if (gport != nullptr)
        {
            // internal entity
            GateTemplatePort_shptr tmpl_port = gport->get_template_port();
            if (tmpl_port->is_outport()) return true;
        }
    }
    return false;
}


std::string gate_port_already_named(degate::Module::port_collection const& ports, degate::GatePort_shptr gate_port)
{
    for (Module::port_collection::const_iterator iter = ports.begin(); iter != ports.end(); ++iter)
    {
        if (iter->second == gate_port) return iter->first;
    }
    return "";
}


void Module::determine_module_ports()
{
    if (is_main_module())
    {
        throw std::logic_error("determine_module_ports() is not suited for main modules. See determine_module_ports_for_root().");
    }

    int pnum = 0;
    port_collection new_ports;
    std::set<Net_shptr> known_net;

    for (auto g_iter = gates_begin(); g_iter != gates_end(); ++g_iter)
    {
        Gate_shptr gate = *g_iter;
        assert(gate != nullptr);

        for (auto p_iter = gate->ports_begin(); p_iter != gate->ports_end(); ++p_iter)
        {
            GatePort_shptr gate_port = *p_iter;
            assert(gate_port != nullptr);

            Net_shptr net = gate_port->get_net();
            std::cout << "Check net for object gate port " << gate_port->get_descriptive_identifier() << "?\n";

            // To process only 1 time a net.
            bool net_already_processed = known_net.find(net) != known_net.end();
            if ((net != nullptr) && !net_already_processed && !net_completely_internal(net))
            {
                bool is_a_port = false;

                for (Net::connection_iterator c_iter = net->begin(); c_iter != net->end() && !is_a_port; ++c_iter)
                {
                    object_id_t oid = *c_iter;

                    if (!exists_gate_port_recursive(oid))
                    {
                        // Outbound connection

                        // Now we check, whether the connection is feeded by an outside entity or feeded
                        // from this module.
                        // Problem: We can't see the object outside this module, because we only have an
                        // object ID and no logic model object to look up the object ID. Therefore we have
                        // to derive the state of feeding from the objects we have in this or any sub-module.
                        // If we see only in-ports in the net, the module port must be driven by an outside
                        // port.

                        GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();
                        assert(tmpl_port != nullptr); // If a gate has no standard cell type, the gate cannot have a port

                        if (net_feeded_internally(net) && tmpl_port->is_inport())
                        {
                            std::cout << "  Net feeded internally, but port is inport. Will check where the net is driven.\n";
                        }
                        else
                        {
                            std::string mod_port_name = gate_port_already_named(ports, gate_port);
                            if (mod_port_name == "")
                            {
                                // Generate a new port name and check if the port name is already in use
                                do
                                {
                                    pnum++;
                                    boost::format f("p%1%");
                                    f % pnum;
                                    mod_port_name = f.str();
                                }
                                while (ports.find(mod_port_name) != ports.end());
                            }

                            std::cout << "  New module port: " << gate_port->get_descriptive_identifier() << " == " << mod_port_name << "\n";
                            new_ports[mod_port_name] = gate_port;

                            is_a_port = true;
                            std::cout << "  Set net as known net: " << net->get_descriptive_identifier() << "\n";
                            known_net.insert(net);
                        }
                    }
                }
            }
            else if (net == nullptr)
            {
                std::cout << "  Will not check net -- no net.\n";
            }
            else if (net_already_processed)
            {
                std::cout << "  Will not check net -- already processed " << net->get_descriptive_identifier() << ".\n";
            }
        }
    }


    // check sub-modules
    for (auto sub : modules)
    {
        for (const auto& p : sub->ports)
        {
            std::string mod_port_name = p.first;
            GatePort_shptr gate_port = p.second;
            Net_shptr net = gate_port->get_net();

            bool net_already_processed = known_net.find(net) != known_net.end();

            if (net != nullptr && !net_already_processed && !net_completely_internal(net))
            {
                // outbound connection
                new_ports[mod_port_name] = gate_port;
            }
        }
    }
    ports = new_ports;
}

void Module::determine_module_ports_recursive()
{
    for (module_collection::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        (*it)->determine_module_ports();
        (*it)->determine_module_ports_recursive();
    }
}

void Module::add_module_port(std::string const& module_port_name, GatePort_shptr adjacent_gate_port)
{
    ports[module_port_name] = adjacent_gate_port;
}

bool Module::exists_gate_port_recursive(object_id_t oid) const
{
    return lookup_gate_port_recursive(oid) != nullptr;
}

GatePort_shptr Module::lookup_gate_port_recursive(object_id_t oid) const
{
    assert(oid != 0);

    for (gate_collection::const_iterator g_iter = gates.begin();
         g_iter != gates.end(); ++g_iter)
    {
        Gate_shptr gate = *g_iter;

        for (Gate::port_const_iterator p_iter = gate->ports_begin();
             p_iter != gate->ports_end(); ++p_iter)
            if ((*p_iter)->get_object_id() == oid) return *p_iter;
    }

    for (module_collection::const_iterator iter = modules.begin();
         iter != modules.end(); ++iter)
        if (GatePort_shptr gport = (*iter)->lookup_gate_port_recursive(oid)) return gport;

    return GatePort_shptr();
}


void degate::determine_module_ports_for_root(LogicModel_shptr lmodel)
{
    debug(TM, "Check for module ports.");
    /*
      Iterate over all gate and check their ports.
      Get the net for a port.
      Iterate over net.
      If a net contains a emarker with a description of 'module-port', use
      gate port as module port.
     */

    Module_shptr main_module = lmodel->get_main_module();

    main_module->ports.clear(); // reset ports

    for (Module::gate_collection::iterator g_iter = main_module->gates_begin();
         g_iter != main_module->gates_end(); ++g_iter)
    {
        Gate_shptr gate = *g_iter;
        assert(gate != nullptr);

        for (Gate::port_const_iterator p_iter = gate->ports_begin(); p_iter != gate->ports_end(); ++p_iter)
        {
            GatePort_shptr gate_port = *p_iter;
            assert(gate_port != nullptr);

            Net_shptr net = gate_port->get_net();
            bool is_a_port = false;

            if (net != nullptr)
            {
                for (Net::connection_iterator c_iter = net->begin(); c_iter != net->end() && !is_a_port; ++c_iter)
                {
                    object_id_t oid = *c_iter;
                    assert(oid != 0);

                    PlacedLogicModelObject_shptr lmo = lmodel->get_object(oid);
                    if (EMarker_shptr em = std::dynamic_pointer_cast<EMarker>(lmo))
                    {
                        debug(TM, "Connected with emarker");

                        if (em->is_module_port() || em->get_description() == "module-port")
                        {
                            GateTemplatePort_shptr tmpl_port = gate_port->get_template_port();

                            // If a gate has no standard cell type, the gate cannot have a port.
                            assert(tmpl_port != nullptr);

                            main_module->ports[em->get_name()] = gate_port;
                            is_a_port = true;
                        }
                    }
                }
            } // end of net-object-iteration
        } // end of gate-portiteration
    } // end of gate-iteration
}


Module_shptr Module::lookup_module(std::string const& module_path) const
{
    std::list<std::string> path_elements;
    boost::split(path_elements, module_path, boost::is_any_of("/"));

    if (is_main_module())
    {
        // handle absolut path
        while (path_elements.size() > 0 &&
            get_name() != path_elements.front())
        {
            path_elements.pop_front();
        }
    }

    if (path_elements.size() == 0)
        return Module_shptr();


    // leading path element should equals current module name
    if (get_name() != path_elements.front())
        return Module_shptr();
    else
    {
        path_elements.pop_front();
        return lookup_module(path_elements); // first real name is name of the main module, skipped
    }
}

Module_shptr Module::lookup_module(std::list<std::string>& path_elements) const
{
    if (path_elements.size() > 0)
    {
        for (auto m : modules)
        {
            if (m->get_name() == path_elements.front())
            {
                if (path_elements.size() == 1)
                    return m;
                else
                {
                    path_elements.pop_front();
                    return m->lookup_module(path_elements);
                }
            }
        }
    }

    return Module_shptr();
}

void Module::set_module_port_name(std::string const& module_port_name, GatePort_shptr adjacent_gate_port)
{
    for (Module::port_collection::const_iterator iter = ports.begin(); iter != ports.end(); ++iter)
    {
        if (iter->second == adjacent_gate_port)
        {
            ports.erase(iter->first);
            ports[module_port_name] = adjacent_gate_port;
            return;
        }
    }
}


boost::optional<std::string> Module::lookup_module_port_name(GatePort_shptr gate_port)
{
    for (Module::port_collection::const_iterator iter = ports.begin(); iter != ports.end(); ++iter)
    {
        if (iter->second == gate_port)
        {
            return boost::optional<std::string>(iter->first);
        }
    }
    return boost::optional<std::string>();
}


bool Module::exists_module_port_name(std::string const& module_port_name) const
{
    return ports.find(module_port_name) != ports.end();
}

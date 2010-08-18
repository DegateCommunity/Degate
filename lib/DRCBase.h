#ifndef __DRCBASE_H__
#define __DRCBASE_H__

#include <boost/foreach.hpp>
#include <tr1/memory>
#include <list>
#include <LogicModel.h>

namespace degate {

  class DRCViolation {
  private:

    PlacedLogicModelObject_shptr _obj;
    std::string _problem_description;
    std::string _drc_violation_class;

  public:
    DRCViolation(PlacedLogicModelObject_shptr obj,
		 std::string const& problem_description,
		 std::string const& drc_violation_class) :
      _obj(obj), 
      _problem_description(problem_description),
      _drc_violation_class(drc_violation_class) {
    }

    std::string get_problem_description() const {
      return _problem_description;
    }

    std::string get_drc_violation_class() const {
      return _drc_violation_class;
    }

    PlacedLogicModelObject_shptr get_object() const {
      return _obj;
    }
  };

  typedef std::tr1::shared_ptr<DRCViolation> DRCViolation_shptr;

  /**
   * Base class for Design Rule Checks.
   */

  class DRCBase {
  public:

    typedef std::list<DRCViolation_shptr> container_type;

  private:
    std::string _class_name;
    std::string _description;

    container_type drc_violations;

  public:

    /**
     * The constructor.
     * @param short_name Short name for the DRC class.
     * @param description A decription of what the DRC basically checks.
     */
    DRCBase(std::string const& class_name,
	    std::string const& description) : 
      _class_name(class_name),
      _description(description) {
    }

    virtual ~DRCBase() {}

    /**
     * The run method is abstract and must be implemented in derived
     * classes. The implementation should check for design rule violations.
     * Each DRC violation must be stored via method add_drc_violation().
     * Note: Because run() can be called multiple times, at the beginning of
     * run() you must clear the list of detected violations.
     */
    virtual void run(LogicModel_shptr lmodel) = 0;

    /**
     * Get the list of DRC violations.
     */

    container_type get_drc_violations() const {
      return drc_violations;
    }

    std::string get_drc_class_name() const {
      return _class_name;
    }

  protected:

    /**
     * Add a DRC violation to the list of already detected violations.
     */
    void add_drc_violation(DRCViolation_shptr violation) {
      drc_violations.push_back(violation);
    }

    /**
     * Clear list of detected violations.
     */
    void clear_drc_violations() {
      drc_violations.clear();
    }
  };

  typedef std::tr1::shared_ptr<DRCBase> DRCBase_shptr;

  /**
   * Design Rule Checks that detects open ports, that means if 
   * a port is electrically unconnected.
   */

  class DRCOpenPorts : public DRCBase {

  public:

    DRCOpenPorts();

    void run(LogicModel_shptr lmodel);
  
  };



  class DesignRuleChecker : public DRCBase {

  private:

    std::list<DRCBase_shptr> checks;

  public:

    DesignRuleChecker() : DRCBase("drc-all", "A collection of all DRCs.") {
      checks.push_back(DRCBase_shptr(new DRCOpenPorts()));
    }

    void run(LogicModel_shptr lmodel) {

      debug(TM, "run DRC");

      clear_drc_violations();

      BOOST_FOREACH(DRCBase_shptr check, checks) {
	std::cout << "DRC: " << check->get_drc_class_name() << std::endl;
	check->run(lmodel);
	BOOST_FOREACH(DRCViolation_shptr violation, check->get_drc_violations()) {
	  add_drc_violation(violation);
	}
      }

      debug(TM, "found %d drc violations.", get_drc_violations().size());
    }
  };

}

#endif

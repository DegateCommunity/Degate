#ifndef __HLOBJECTSET_H__
#define __HLOBJECTSET_H__

#include <degate.h>
#include <set>
#include <list>
#include <tr1/memory>

class HlObjectSet {

  
 private:
  std::set<std::tr1::shared_ptr<degate::PlacedLogicModelObject> > objects;

 public: 
  void clear();
  void highlight(bool state);
  void add(std::tr1::shared_ptr<degate::PlacedLogicModelObject> object);
  void remove(std::tr1::shared_ptr<degate::PlacedLogicModelObject> object);
};

#endif

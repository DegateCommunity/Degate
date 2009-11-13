#include "HlObjectSet.h"
#include <algorithm>
#include <tr1/memory>

using namespace std;
using namespace degate;


void HlObjectSet::clear() {
  highlight(false);
  objects.clear();
}

void HlObjectSet::highlight(bool state = true) {

  set<std::tr1::shared_ptr<PlacedLogicModelObject> >::iterator it;

  for(it = objects.begin(); it != objects.end(); ++it) {
    (*it)->set_selected(state);
  } 
}


void HlObjectSet::add(std::tr1::shared_ptr<PlacedLogicModelObject> object) {
  object->set_selected();
  objects.insert(object);
}


void HlObjectSet::remove(std::tr1::shared_ptr<PlacedLogicModelObject> object) {

  set<std::tr1::shared_ptr<PlacedLogicModelObject> >::iterator it = 
    find(objects.begin(), objects.end(), object);

  if(it != objects.end()) {
    
    (*it)->set_selected(false);
    objects.erase(it);
  }

}

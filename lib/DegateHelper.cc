#include <DegateHelper.h>
#include <vector>
#include <string>
#include <iostream>

using namespace degate;
using namespace std;

std::vector<std::string> degate::tokenize(std::string const& str) {

  /* This implementation is more or less directly derived from
     this posting http://www.gamedev.net/community/forums/topic.asp?topic_id=320087
    
  */
  vector<string> result;

  string          item;
  stringstream    ss(str);

  while(ss >> item){
    if (item[0]=='"') {
      int lastItemPosition = item.length() - 1;
      if (item[lastItemPosition]!='"') {
        // read the rest of the double-quoted item
        string restOfItem;
        getline(ss, restOfItem, '"');
        item += restOfItem;
      }
      // otherwise, we had a single word that was quoted. In any case, we now
      // have the item in quotes; remove them
      item = item.substr(1, lastItemPosition-1);
    }
    // item is "fully cooked" now
    result.push_back(item);
  }
  return result;


}

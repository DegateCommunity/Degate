#include <degate.h>
#include <DegateHelper.h>
#include <Configuration.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>

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

void degate::write_string_to_file(std::string const& path,
				  std::string const& content) {

  std::ofstream file;
  file.open(path.c_str(), std::ios::trunc | std:: ios::out);
  file << content;
  file.close();
}

std::string degate::get_data_dir() {
  Configuration const & conf = Configuration::get_instance();
  return conf.get_data_dir();
}

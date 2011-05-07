#include <degate.h>
#include <DegateHelper.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/foreach.hpp>

#include <sys/wait.h>

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

std::string degate::write_string_to_temp_file(std::string const& dir,
					      std::string const& content) {

  char filename[PATH_MAX];
  std::string pattern = generate_temp_file_pattern(dir);
  strncpy(filename, pattern.c_str(), sizeof(filename));
  if(!mktemp(filename)) // should never return NULL
    throw DegateRuntimeException("mktemp() failed");

  write_string_to_file(filename, content);

  return filename;
}

void degate::write_string_to_file(std::string const& path,
				  std::string const& content) {

  std::ofstream file;
  file.open(path.c_str(), std::ios::trunc | std:: ios::out);
  file << content;
  file.close();
}

int degate::execute_command(std::string const& command, std::list<std::string> const& params) {

  pid_t pid = fork();
  if(pid == 0) {
    // child
    std::cout << "Execute command " << command << " ";
    char const ** argv = new char const *[params.size()+2];
    int i = 1;
    BOOST_FOREACH(std::string const& s, params) {
      argv[i] = s.c_str();
      i++;
      std::cout << s << " ";
    }
    argv[0] = command.c_str();
    argv[i] = NULL;

    std::cout << std::endl;

    if(execvp(command.c_str(), const_cast<char* const*>(argv)) == -1) {
      std::cout << "exec failed" << std::endl;
    }
    std::cout << "sth. failed" << std::endl;
    exit(0);
  }
  else if(pid < 0) {
    // fork failed
    throw SystemException("fork() failed");
  }
  else {

    // parent
    int exit_code;
    if(waitpid(pid, &exit_code, 0) != pid)
      throw SystemException("waitpid() failed");
    else {
      if(WEXITSTATUS(exit_code) != 0) {
	std::string errmsg("Failed to execute command: " + command);
        BOOST_FOREACH(std::string const& s, params) {
          errmsg += " ";
          errmsg += s;
        }
        errmsg += ". Error: ";
        errmsg += strerror(errno);
        throw SystemException(errmsg);
      }
      return  exit_code;    
    }
  }
}


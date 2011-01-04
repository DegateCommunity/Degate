#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <tr1/memory>


template <typename FunctionType>
class ThreadPool {

  typedef std::tr1::shared_ptr<boost::thread> thread_shptr;

private:
  unsigned int max_n;

  std::list<FunctionType> task_queue;
  std::list<thread_shptr> running;

  void spawn() {
      while(running.size() < max_n && task_queue.size() > 0 ) {
	FunctionType f(task_queue.front());
	task_queue.pop_front();
	
	boost::thread * p = new boost::thread(f);
	thread_shptr t = thread_shptr(p);
	running.push_back(t);
      }
  }

public:

  ThreadPool(unsigned int n = 4) : max_n(n) {
  }

  ~ThreadPool() {
    wait();
  }


  void add(FunctionType f) {
    task_queue.push_back(f);
  }

  void wait() {

    while(task_queue.size() > 0 || running.size() > 0) { 

      spawn();
    
      for(std::list<thread_shptr>::iterator iter = running.begin();
	  iter != running.end(); ++iter) {
	
	//std::cout << "timed wait\n";
	if((*iter)->timed_join(boost::posix_time::millisec( 1000 )   )) {
	  std::list<thread_shptr>::iterator i(iter);
	  ++iter;
	  running.erase(i);
	}
	
      }


    }

  }


};

#endif

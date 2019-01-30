#ifndef PYSCHEDULE_LIB_INCLUDE_
#define PYSCHEDULE_LIB_INCLUDE_

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/export.hpp>

#include <iostream>
#include <thread>
#include <memory>
#include <queue>
#include <vector>
#include <map>
#include <sstream>
#include <type_traits>
#include <chrono>
#include <mutex>
#include <condition_variable>

using namespace std::chrono;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&); \
  void operator=(const TypeName&)

#endif //PYSCHEDULE_LIB_INCLUDE_

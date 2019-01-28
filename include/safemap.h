#ifndef PYSCHEDULE_SAFEMAP_H_
#define PYSCHEDULE_SAFEMAP_H_

#include "include/lib_include.h"

template <class K, class T>
class SafeMap {
 public:
  SafeMap(): map_(), mutex_() {};
  void insert(K k, T t) {
    boost::mutex::scoped_lock lock(mutex_);
    map_.insert({k, t});
  }
  // dft: default value
  T erase(K k) {
    boost::mutex::scoped_lock lock(mutex_);
    auto it = map_.find(k);
    T ret = it->second;
    map_.erase(it);
    return ret;
  }

 private:
  std::map<K, T> map_;
  mutable boost::mutex mutex_;

  DISALLOW_COPY_AND_ASSIGN(SafeMap);
};

#endif // PYSCHEDULE_SAFEMAP_H_

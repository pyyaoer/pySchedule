#ifndef PYSCHEDULE_SAFELIST_H_
#define PYSCHEDULE_SAFELIST_H_

#include "include/lib_include.h"

template <class T>
class SafeList {
  using RT = typename std::remove_reference<decltype(*std::declval<T>())>::type;

 public:
  SafeList(): list_(), mutex_() {};
  void push(T t) {
    boost::mutex::scoped_lock lock(mutex_);
    list_.push_back(t);
  }

  bool erase_match(RT &ret, std::function<bool(RT)> const& p) {
    boost::mutex::scoped_lock lock(mutex_);
    for (auto i = list_.begin(); i != list_.end(); i++) {
      ret = std::move(**i);
      if (p(ret)) {
        list_.erase(i);
        return true;
      }
    }
    return false;
  }

  int size() {
    boost::mutex::scoped_lock lock(mutex_);
    return list_.size();
  }

 private:
  std::vector<T> list_;
  mutable boost::mutex mutex_;

  DISALLOW_COPY_AND_ASSIGN(SafeList);
};

#endif // PYSCHEDULE_SAFELIST_H_

#ifndef PYSCHEDULE_SAFEQUEUE_H_
#define PYSCHEDULE_SAFEQUEUE_H_

#include "include/lib_include.h"

template <class T>
class SafeQueue {
 public:
  SafeQueue() {};
  void push(T t) {
    std::lock_guard<std::mutex> guard(mutex_);
    queue_.push(t);
  }
  // dft: default value
  T pop(T dft) {
    std::lock_guard<std::mutex> guard(mutex_);
    T t = dft;
    if (not queue_.empty()) {
      t = queue_.front();
      queue_.pop();
    }
    return t;
  }

 private:
  std::mutex mutex_;
  std::queue<T> queue_;

  DISALLOW_COPY_AND_ASSIGN(SafeQueue);
};

#endif // PYSCHEDULE_SAFEQUEUE_H_

#ifndef PYSCHEDULE_SAFEQUEUE_H_
#define PYSCHEDULE_SAFEQUEUE_H_

#include "include/lib_include.h"

template <class T>
class SafeQueue {
 public:
  SafeQueue(): queue_(), mutex_(), cond_() {};
  void push(T t) {
    boost::mutex::scoped_lock lock(mutex_);
    queue_.push(t);
    cond_.notify_one();
  }
  // dft: default value
  T pop() {
    boost::mutex::scoped_lock lock(mutex_);
    while (queue_.empty()) {
      cond_.wait(lock);
    }
    T t = queue_.front();
    queue_.pop();
    return t;
  }

 private:
  std::queue<T> queue_;
  mutable boost::mutex mutex_;
  boost::condition_variable cond_;

  DISALLOW_COPY_AND_ASSIGN(SafeQueue);
};

#endif // PYSCHEDULE_SAFEQUEUE_H_

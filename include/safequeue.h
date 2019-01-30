#ifndef PYSCHEDULE_SAFEQUEUE_H_
#define PYSCHEDULE_SAFEQUEUE_H_

#include "include/lib_include.h"

template <class T>
class SafeQueue {
 public:
  SafeQueue(): queue_(), mutex_(), cond_() {};
  void push(T t) {
    std::unique_lock lock(mutex_);
    queue_.push_front(t);
    cond_.notify_one();
  }

  T pop() {
    std::unique_lock lock(mutex_);
    while (queue_.empty()) {
      cond_.wait(lock);
    }
    T t = queue_.back();
    queue_.pop_back();
    return t;
  }

  T back() {
    std::unique_lock lock(mutex_);
    return queue_.back();
  }

  bool empty() {
    std::unique_lock lock(mutex_);
    return queue_.empty();
  }

  int size() {
    std::unique_lock lock(mutex_);
    return queue_.size();
  }

 private:
  std::deque<T> queue_;
  mutable std::mutex mutex_;
  std::condition_variable cond_;

  DISALLOW_COPY_AND_ASSIGN(SafeQueue);
};

#endif // PYSCHEDULE_SAFEQUEUE_H_

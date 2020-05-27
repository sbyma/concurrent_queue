
// Concurrent Queue
// Stuart Byma
// Contributions from Sam Whitlock

/* Copyright 2020 Stuart Byma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

// Blocking/NonBlocking Concurrent queue (bcq) based on std::queue
// thread-safe with limited buffer capacity

namespace bcq {
template <typename T>
class ConcurrentQueue {
 public:
  ConcurrentQueue(size_t capacity) : capacity_(capacity) {}

  // return true if pushed, false otherwise
  // will block until pushed if block_ is true
  bool Push(const T& item);
  bool Push(T&& item);

  // return true if success and item is valid, false otherwise
  // move()s result into item
  bool Pop(T& item);

  // peek (get a copy of) the top item in the queue
  // requires a copyable type
  bool Peek(T& item);

  // unblock the queue, notify all threads
  void UnBlock();

  // set blocking behavior (default)
  void SetBlock();

  bool Empty() const;
  size_t Capacity() const;
  size_t Size() const;

 private:
  std::mutex mu_;

  // cond vars for block/wait/notify on queue push/pop
  std::condition_variable queue_pop_cv_;
  std::condition_variable queue_push_cv_;

  std::queue<T> queue_;

  size_t capacity_;

  bool block_ = true;
};

template <typename T>
bool ConcurrentQueue<T>::Peek(T& item) {
  bool popped = false;
  {
    std::unique_lock<std::mutex> l(mu_);

    if (queue_.empty() && block_) {
      while (queue_.empty() && block_) {
        queue_pop_cv_.wait(l);
      }
    }

    if (!queue_.empty()) {
      item = queue_.front();
      popped = true;
    }
  }

  if (popped) {
    queue_pop_cv_.notify_one();
  }

  return popped;
}

template <typename T>
bool ConcurrentQueue<T>::Pop(T& item) {
  bool popped = false;
  {
    std::unique_lock<std::mutex> l(mu_);

    if (queue_.empty() && block_) {
      while (queue_.empty() && block_) {
        queue_pop_cv_.wait(l);
      }
    }

    if (!queue_.empty()) {
      item = std::move(queue_.front());
      queue_.pop();
      popped = true;
    }
  }
  if (popped) {
    // tell someone blocking on write they can now write to the queue
    queue_push_cv_.notify_one();
    return true;
  } else
    return false;
}

template <typename T>
bool ConcurrentQueue<T>::Push(const T& item) {
  bool pushed = false;
  {
    std::unique_lock<std::mutex> l(mu_);
    // we block until something pops and makes room for us
    // unless blocking is set to false
    if (queue_.size() == capacity_ && block_) {
      while (queue_.size() == capacity_ && block_) {
        queue_push_cv_.wait(l);
      }
    }

    if (queue_.size() < capacity_) {
      queue_.push_back(item);
      pushed = true;
    }
  }

  if (pushed) {
    queue_pop_cv_.notify_one();
    return true;
  } else {
    return false;
  }
}

template <typename T>
bool ConcurrentQueue<T>::Push(T&& item) {
  bool pushed = false;
  {
    std::unique_lock<std::mutex> l(mu_);
    // we block until something pops and makes room for us
    // unless blocking is set to false
    if (queue_.size() == capacity_ && block_) {
      while (queue_.size() == capacity_ && block_) {
        queue_push_cv_.wait(l);
      }
    }

    if (queue_.size() < capacity_) {
      queue_.push(std::move(item));
      pushed = true;
    }
  }

  if (pushed) {
    queue_pop_cv_.notify_one();
    return true;
  } else {
    return false;
  }
}

template <typename T>
void ConcurrentQueue<T>::UnBlock() {
  {
    std::unique_lock<std::mutex> l(mu_);
    block_ = false;
  }

  queue_push_cv_.notify_all();
  queue_pop_cv_.notify_all();
}

template <typename T>
void ConcurrentQueue<T>::SetBlock() {
  std::unique_lock<std::mutex> l(mu_);
  block_ = true;
}

template <typename T>
bool ConcurrentQueue<T>::Empty() const {
  return queue_.empty();
}

template <typename T>
size_t ConcurrentQueue<T>::Capacity() const {
  return capacity_;
}

template <typename T>
size_t ConcurrentQueue<T>::Size() const {
  return queue_.size();
}

}  // namespace bcq

#pragma once
#include <memory>
#include <mutex>
#include <utility>
template<typename T>
class threadsafe_queue {
public:
  threadsafe_queue():
    head_(new node_), tail_(head_.get()) {}
  threadsafe_queue(const threadsafe_queue& other) = delete;
  threadsafe_queue& operator=(const threadsafe_queue& other) = delete;

  void Push(T new_value);
  std::shared_ptr<T> WaitAndPop();
  void WaitAndPop(T& value);
  std::shared_ptr<T> TryPop();
  bool TryPop(T& value);
  void Empty();
  void NotifyOther();

private:
  bool other_notification_;
  mutable std::mutex head_mutex_;
  mutable std::mutex tail_mutex_;
  node_* tail_;
  std::condition_variable data_cond_;
  std::unique_ptr<node_> head_;
  struct node_ {
    std::shared_ptr<T> data_;
    std::unique_ptr<node_> next_;
  };

  node_* GetTail_() {
    std::lock_guard<std::mutex> tail_lock(tail_mutex_);
    return tail_;
  }
  std::unique_ptr<node_> PopHead_() {
    std::unique_ptr<node_> old_head = std::move(head_);
    head_ = std::move(old_head->next_);
    return old_head;
  }
  std::unique_lock<std::mutex> WaitForData_() {
    std::unique_lock<std::mutex> head_lock(head_mutex_);
    data_cond_.wait(head_lock, [&]() {return head_.get() != GetTail_(); });
    return std::move(head_lock);
  }
  std::unique_ptr<node_> WaitPopHead_() {
    std::unique_lock<std::mutex> head_lock(WaitForData_());
    return PopHead_();
  }
  std::unique_ptr<node_> WaitPopHead_(T& value) {
    std::unique_lock<std::mutex> head_lock(WaitForData_());
    value = std::move(*head_->data_);
    return PopHead_();
  }
  std::unique_ptr<node_> TryPopHead_() {
    std::lock_guard<std::mutex> head_lock(head_mutex_);
    if (head_.get() == GetTail_()) {
      return std::unique_ptr<node_>();
    }
    return PopHead_();
  }
  std::unique_ptr<node_> TryPopHead_(T& value) {
    std::lock_guard<std::mutex> head_lock(head_mutex_);
    if (head_.get() == GetTail_()) {
      return std::unique_ptr<node_>();
    }
    value = std::move(*head_->data_);
    return PopHead_();
  }
};

template<typename T>
void threadsafe_queue<T>::Push(T new_value) {
  std::shared_ptr<T> new_data(
    std::make_shared<T>(std::move(new_value)));
  std::unique_ptr<node_> p(new node_);
  {
    std::lock_guard<std::mutex> tail_lock(tail_mutex_);
    tail_->data_ = new_data;
    node_* const new_tail = p.get();
    tail_->next_ = std::move(p);
    tail_ = new_tail;
  }
  data_cond_.notify_one();
}

template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::WaitAndPop() {
  std::unique_ptr<node_> const old_head = WaitPopHead_();
  return old_head->data_;
}

template<typename T>
void threadsafe_queue<T>::WaitAndPop(T& value) {
  std::unique_ptr<node_> const old_head = WaitPopHead_(value);
}

template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::TryPop() {
  std::unique_ptr<node_> old_head = TryPopHead_();
  return old_head ? old_head->data_ : std::shared_ptr<T>();
}

template<typename T>
bool threadsafe_queue<T>::TryPop(T& value) {
  std::unique_ptr<node_> const old_head = TryPopHead_(value);
  return old_head;
}

template<typename T>
void threadsafe_queue<T>::Empty() {
  std::lock_guard<std::mutex> head_lock(head_mutex_);
  return (head_.get() == GetTail_());
}

template<typename T>
void threadsafe_queue<T>::NotifyOther() {
  other_notification_ = true;
  data_cond_.notify_all();
}

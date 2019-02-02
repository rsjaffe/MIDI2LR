#include "Proxy.h"
#include <mutex>

Proxy::Proxy() : data_(Make_Impl()) {}

std::shared_ptr<Proxy::Impl> Proxy::Make_Impl() const
{ // note that impls_ may be accessed by another thread with non-const member function
  // (assignment) if it is running slightly ahead of this thread with an uninitialized impls_ so
  // need mutex before shared_ptr.lock call. if we use an atomic weak pointer (C++20) can call
  // shared_ptr.lock before mutex, check for success, and only run mutex (and try lock again) after
  // failure
   static std::mutex mtx;
   auto lock = std::lock_guard(mtx);
   std::shared_ptr<Impl> internal_data = impls_.lock();
   if (internal_data)
      return internal_data;
   const auto new_impl(std::make_shared<Impl>());
   impls_ = new_impl;
   return new_impl;
}
Proxy::Impl::Impl()
{
   // implement constructor for Impl here
}
void Proxy::Impl::method1()
{
   // here is the real method1 for the implementation
}

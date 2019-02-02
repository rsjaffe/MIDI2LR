#include "Scrimpl.h"
#include <mutex>

class Scrimpl::Impl {
 public:
   Impl();
   ~Impl() = default;
};

Scrimpl::Impl::Impl()
{
   // implement constructor for Impl here
}

Scrimpl::Scrimpl() : data_(Make_Impl()) {}

std::shared_ptr<const Scrimpl::Impl> Scrimpl::Make_Impl()
{ // note that impls_ may be accessed by another thread with non-const member function
  // (assignment) if it is running slightly ahead of this thread on uninitialized impls_ so need
  // mutex before lock call. if we use an atomic weak pointer (C++20) can call lock before mutex,
  // check for success, and only run mutex (and try lock again) after failure
   static std::mutex mtx;
   auto lock = std::lock_guard(mtx);
   std::shared_ptr<const Impl> internal_data = impls_.lock();
   if (!internal_data) {
      const auto new_impl(std::make_shared<const Impl>());
      internal_data = new_impl;
      impls_ = new_impl;
   }
   return internal_data;
}

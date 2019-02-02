#pragma once
#ifndef MIDI2LR_SCRIMPL_H_INCLUDED
#include <memory>
class Proxy {
 public:
   Proxy();
   void method1() // inline dispatch to hidden single class
   {
      data_->method1();
   }

 private:
   class Impl {
    public:
      void method1();
      Impl();
      ~Impl() = default;
   };
   std::shared_ptr<Impl> Make_Impl() const;
   const std::shared_ptr<Impl> data_{};
   inline static std::weak_ptr<Impl> impls_{};
};

class Proxy2 {
 public:
   Proxy2() : m_impl(make_impl()) {}

   void method()
   {
      m_impl.method();
   }

 private:
   class Impl {
    public:
      Impl() {}
      ~Impl() {}
      void method() {}

    private:
   };

   Impl& m_impl;
   Impl& make_impl()
   {
      static Impl singleimpl;
      return singleimpl;
   }
};
#endif // MIDI2LR_SCRIMPL_H_INCLUDED
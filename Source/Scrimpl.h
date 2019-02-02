#pragma once
#ifndef MIDI2LR_SCRIMPL_H_INCLUDED
#include <memory>
class Scrimpl {
 public:
   Scrimpl();

 private:
   class Impl; // public here because of Cereal requirements. otherwise private
   std::shared_ptr<const Impl> Make_Impl();
   std::shared_ptr<const Impl> data_;
   static std::weak_ptr<const Impl> impls_;
};
#endif // MIDI2LR_SCRIMPL_H_INCLUDED
#pragma once
/*
==============================================================================

Utilities.h
This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
==============================================================================
*/
#include <atomic>
namespace MIDI2LR {

  template <typename T>
  struct counter {
    static std::atomic_int objects_created;
    static std::atomic_int objects_alive;

    counter() noexcept {
      ++objects_created;
      ++objects_alive;
    }

    counter(const counter&) noexcept {
      ++objects_created;
      ++objects_alive;
    }
  protected:
    ~counter() // objects should never be removed through pointers of this type
    {
      --objects_alive;
    }
  };
  template <typename T> std::atomic_int counter<T>::objects_created(0);
  template <typename T> std::atomic_int counter<T>::objects_alive(0);
}
/*
Usage:

class X :  MIDI2LR::counter<X>
{
// ...
};

*/

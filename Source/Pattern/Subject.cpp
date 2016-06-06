/*
  ==============================================================================

    Subject.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later 
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
   ==============================================================================
*/

#include "Subject.h"

Subject::Subject() noexcept {}

Subject::~Subject() {}

void Subject::RegisterObserver(Observer * observer) {
    //add to the vector
  observers_.push_back(observer);
}

void Subject::UnregisterObserver(Observer *observer) {
    // using the for loop instead of the for ( auto &i : v ) as items will be removed
  for (vector<Observer *>::iterator it = observers_.begin(); it != observers_.end(); ++it) {
      //check if the pointers are the same
    if ((*it) == observer) {
        //erase and assign the iterator to the new position
      it = observers_.erase(it);
    }
  }
}

void Subject::Notify(void) {
    //check if the array is not empty.
  if (!observers_.empty()) {
  }
}
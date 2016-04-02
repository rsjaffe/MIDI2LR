/*
  ==============================================================================

    Subject.cpp

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

#include "Subject.h"

/**********************************************************************************************//**
 * @fn  Subject::Subject() noexcept
 *
 * @brief   Default constructor.
 *
 *
 **************************************************************************************************/

Subject::Subject() noexcept
{
}

/**********************************************************************************************//**
 * @fn  Subject::~Subject()
 *
 * @brief   Destructor.
 *
 *
 *
 **************************************************************************************************/

Subject::~Subject()
{
}

/**********************************************************************************************//**
 * @fn  void Subject::RegisterObserver(Observer * observer)
 *
 * @brief   Registers the observer described by observer.
 *
 *
 *
 *
 * @param [in,out]  observer    If non-null, the observer.
 **************************************************************************************************/

void Subject::RegisterObserver(Observer * observer)
{
	//add to the vector
	m_observerArray.push_back(observer);
}

/**********************************************************************************************//**
 * @fn  void Subject::UnregisterObserver(Observer *observer)
 *
 * @brief   Unregisters the observer described by observer.
 *
 *
 *
 *
 * @param [in,out]  observer    If non-null, the observer.
 **************************************************************************************************/

void Subject::UnregisterObserver(Observer *observer)
{
	// using the for loop instead of the for ( auto &i : v ) as items will be removed
	for (vector<Observer *>::iterator it = m_observerArray.begin(); it != m_observerArray.end(); ++it)
	{
		//check if the pointers are the same
		if ((*it) == observer)
		{
			//erase and assign the iterator to the new position
			it = m_observerArray.erase(it);
		}
	}
}

/**********************************************************************************************//**
 * @fn  void Subject::Notify(void)
 *
 * @brief   Notifies this object.
 *
 *
 *
 **************************************************************************************************/

void Subject::Notify(void)
{
	//check if the array is not empty.
	if (!m_observerArray.empty())
	{

	}
}

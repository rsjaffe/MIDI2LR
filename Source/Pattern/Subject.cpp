/*
  ==============================================================================

    Subject.cpp
    Created: 23 Jan 2016 9:00:58am
    Author:  Jeffrey

  ==============================================================================
*/

#include "Subject.h"

Subject::Subject()
{
}

Subject::~Subject()
{
}

void Subject::RegisterObserver(Observer * observer)
{
	//add to the vector
	m_observerArray.push_back(observer);
}

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

void Subject::Notify(void)
{
	//check if the array is not empty.
	if (!m_observerArray.empty())
	{

	}
}

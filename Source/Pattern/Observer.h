/*
  ==============================================================================

    Observer.h
    Created: 23 Jan 2016 9:01:16am
    Author:  Jeffrey

  ==============================================================================
*/

#ifndef OBSERVER_H_INCLUDED
#define OBSERVER_H_INCLUDED

#include "Subject.h"

// forward declaration of the subject interface
class Subject;

class Observer
{
public:
	Observer() {}
	virtual ~Observer() {}
	// the changed function is pure virtual as the overloaded object needs to implement it.
	virtual void Changed(Subject *changed) = 0;
protected:
private:
};



#endif  // OBSERVER_H_INCLUDED

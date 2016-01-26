/*
  ==============================================================================

    Subject.h
    Created: 23 Jan 2016 9:00:58am
    Author:  Jeffrey

  ==============================================================================
*/

#ifndef SUBJECT_H_INCLUDED
#define SUBJECT_H_INCLUDED

#include "Observer.h"
#include <vector>

using namespace std;

class Observer;

class Subject
{
public:
	Subject();
	virtual ~Subject();	
	void RegisterObserver(Observer *observer);
	void UnregisterObserver(Observer *observer);
protected:
	void Notify(void);
private:
	vector<Observer *> m_observerArray;
};





#endif  // SUBJECT_H_INCLUDED

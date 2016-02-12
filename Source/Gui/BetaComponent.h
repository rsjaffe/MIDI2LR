/*
  ==============================================================================

    BetaComponent.h
    Created: 2 Feb 2016 8:40:40pm
    Author:  Jeffrey

  ==============================================================================
*/

#ifndef BETACOMPONENT_H_INCLUDED
#define BETACOMPONENT_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/

#include "../ResizableLayout.h"

// In the beta Component the new features are added to be tested.
// Once a feature is out of the beta stage it must be removed from this component.

class BetaComponent    : public Component, public ButtonListener, ResizableLayout
{
public:
    BetaComponent();
    ~BetaComponent();

    void paint (Graphics&);
    void resized();
	void Init(void);

	// ButtonListener interface
	virtual void buttonClicked(Button* button) override;

private:
	TextButton m_test;
	Label m_explainToEndUser;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BetaComponent)
};


#endif  // BETACOMPONENT_H_INCLUDED

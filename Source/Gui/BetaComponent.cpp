/*
  ==============================================================================

    BetaComponent.cpp
    Created: 2 Feb 2016 8:40:40pm
    Author:  Jeffrey

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "BetaComponent.h"
#include "KeySender.h"

#define BETA_LEFT 20
#define BETA_WIDTH 400
#define BETA_HEIGHT 300

//==============================================================================
BetaComponent::BetaComponent() : ResizableLayout(this), m_test("Test"), m_explainToEndUser()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

BetaComponent::~BetaComponent()
{
}

void BetaComponent::paint (Graphics& g)
{
 

    g.fillAll (Colours::white);   // clear the background

  
}

void BetaComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void BetaComponent::Init(void)
{
	m_explainToEndUser.setFont(Font(12.f, Font::bold));
	m_explainToEndUser.setText("Thank you for testing this functionality\n\
Pressing the test button will should trigger:\n\
1. Focus on Lightroom\n\
2. Sending <ctrl> + L (Library filters ON/OFF)\n\
\n\
Tested on Win7x64 + Lightroom 6.3.\n\
\n\n\
Please let us know your configuration if it does not work.\n\
Known issues: No Mac support.", NotificationType::dontSendNotification);

	m_explainToEndUser.setBounds(BETA_LEFT, 15, BETA_WIDTH - 2 * BETA_LEFT, 150);
	addToLayout(&m_explainToEndUser, anchorMidLeft, anchorMidRight);
	m_explainToEndUser.setEditable(false);
	m_explainToEndUser.setColour(Label::textColourId, Colours::darkgrey);
	addAndMakeVisible(m_explainToEndUser);

	// Remove row button
	m_test.addListener(this);
	m_test.setBounds(BETA_LEFT, BETA_HEIGHT - 50, BETA_WIDTH - BETA_LEFT * 2, 40);
	addToLayout(&m_test, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(m_test);		
}

void BetaComponent::buttonClicked(Button * button)
{
	if (button == &m_test)
	{
		KeySender sendKey;

		sendKey.Configure(false, false, true, 'L');
		sendKey.Execute();

	}
}

/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

struct PropertyPanel::SectionComponent  : public Component
{
    SectionComponent (const String& sectionTitle,
                      const Array<PropertyComponent*>& newProperties,
                      const bool sectionIsOpen)
        : Component (sectionTitle),
          titleHeight (sectionTitle.isNotEmpty() ? 22 : 0),
          isOpen (sectionIsOpen)
    {
        propertyComps.addArray (newProperties);

        for (int i = propertyComps.size(); --i >= 0;)
        {
            addAndMakeVisible (propertyComps.getUnchecked(i));
            propertyComps.getUnchecked(i)->refresh();
        }
    }

    ~SectionComponent()
    {
        propertyComps.clear();
    }

    void paint (Graphics& g) override
    {
        if (titleHeight > 0)
            getLookAndFeel().drawPropertyPanelSectionHeader (g, getName(), isOpen, getWidth(), titleHeight);
    }

    void resized() override
    {
        int y = titleHeight;

        for (int i = 0; i < propertyComps.size(); ++i)
        {
            PropertyComponent* const pec = propertyComps.getUnchecked (i);
            pec->setBounds (1, y, getWidth() - 2, pec->getPreferredHeight());
            y = pec->getBottom();
        }
    }

    int getPreferredHeight() const
    {
        int y = titleHeight;

        if (isOpen)
            for (int i = propertyComps.size(); --i >= 0;)
                y += propertyComps.getUnchecked(i)->getPreferredHeight();

        return y;
    }

    void setOpen (const bool open)
    {
        if (isOpen != open)
        {
            isOpen = open;

            for (int i = propertyComps.size(); --i >= 0;)
                propertyComps.getUnchecked(i)->setVisible (open);

            if (PropertyPanel* const pp = findParentComponentOfClass<PropertyPanel>())
                pp->resized();
        }
    }

    void refreshAll() const
    {
        for (int i = propertyComps.size(); --i >= 0;)
            propertyComps.getUnchecked (i)->refresh();
    }

    void mouseUp (const MouseEvent& e) override
    {
        if (e.getMouseDownX() < titleHeight
              && e.x < titleHeight
              && e.getNumberOfClicks() != 2)
            mouseDoubleClick (e);
    }

    void mouseDoubleClick (const MouseEvent& e) override
    {
        if (e.y < titleHeight)
            setOpen (! isOpen);
    }

    OwnedArray<PropertyComponent> propertyComps;
    const int titleHeight;
    bool isOpen;

    JUCE_DECLARE_NON_COPYABLE (SectionComponent)
};

//==============================================================================
struct PropertyPanel::PropertyHolderComponent  : public Component
{
    PropertyHolderComponent() {}

    void paint (Graphics&) override {}

    void updateLayout (int width)
    {
        int y = 0;

        for (int i = 0; i < sections.size(); ++i)
        {
            SectionComponent* const section = sections.getUnchecked(i);

            section->setBounds (0, y, width, section->getPreferredHeight());
            y = section->getBottom();
        }

        setSize (width, y);
        repaint();
    }

    void refreshAll() const
    {
        for (int i = 0; i < sections.size(); ++i)
            sections.getUnchecked(i)->refreshAll();
    }

    void insertSection (int indexToInsertAt, SectionComponent* newSection)
    {
        sections.insert (indexToInsertAt, newSection);
        addAndMakeVisible (newSection, 0);
    }

    SectionComponent* getSectionWithNonEmptyName (const int targetIndex) const noexcept
    {
        for (int index = 0, i = 0; i < sections.size(); ++i)
        {
            SectionComponent* const section = sections.getUnchecked (i);

            if (section->getName().isNotEmpty())
                if (index++ == targetIndex)
                    return section;
        }

        return nullptr;
    }

    OwnedArray<SectionComponent> sections;

    JUCE_DECLARE_NON_COPYABLE (PropertyHolderComponent)
};


//==============================================================================
PropertyPanel::PropertyPanel()
{
    init();
}

PropertyPanel::PropertyPanel (const String& name)  : Component (name)
{
    init();
}

void PropertyPanel::init()
{
    messageWhenEmpty = TRANS("(nothing selected)");

    addAndMakeVisible (viewport);
    viewport.setViewedComponent (propertyHolderComponent = new PropertyHolderComponent());
    viewport.setFocusContainer (true);
}

PropertyPanel::~PropertyPanel()
{
    clear();
}

//==============================================================================
void PropertyPanel::paint (Graphics& g)
{
    if (isEmpty())
    {
        g.setColour (Colours::black.withAlpha (0.5f));
        g.setFont (14.0f);
        g.drawText (messageWhenEmpty, getLocalBounds().withHeight (30),
                    Justification::centred, true);
    }
}

void PropertyPanel::resized()
{
    viewport.setBounds (getLocalBounds());
    updatePropHolderLayout();
}

//==============================================================================
void PropertyPanel::clear()
{
    if (! isEmpty())
    {
        propertyHolderComponent->sections.clear();
        updatePropHolderLayout();
    }
}

bool PropertyPanel::isEmpty() const
{
    return propertyHolderComponent->sections.size() == 0;
}

int PropertyPanel::getTotalContentHeight() const
{
    return propertyHolderComponent->getHeight();
}

void PropertyPanel::addProperties (const Array<PropertyComponent*>& newProperties)
{
    if (isEmpty())
        repaint();

    propertyHolderComponent->insertSection (-1, new SectionComponent (String(), newProperties, true));
    updatePropHolderLayout();
}

void PropertyPanel::addSection (const String& sectionTitle,
                                const Array<PropertyComponent*>& newProperties,
                                const bool shouldBeOpen,
                                const int indexToInsertAt)
{
    jassert (sectionTitle.isNotEmpty());

    if (isEmpty())
        repaint();

    propertyHolderComponent->insertSection (indexToInsertAt, new SectionComponent (sectionTitle, newProperties, shouldBeOpen));
    updatePropHolderLayout();
}

void PropertyPanel::updatePropHolderLayout() const
{
    const int maxWidth = viewport.getMaximumVisibleWidth();
    propertyHolderComponent->updateLayout (maxWidth);

    const int newMaxWidth = viewport.getMaximumVisibleWidth();
    if (maxWidth != newMaxWidth)
    {
        // need to do this twice because of scrollbars changing the size, etc.
        propertyHolderComponent->updateLayout (newMaxWidth);
    }
}

void PropertyPanel::refreshAll() const
{
    propertyHolderComponent->refreshAll();
}

//==============================================================================
StringArray PropertyPanel::getSectionNames() const
{
    StringArray s;

    for (int i = 0; i < propertyHolderComponent->sections.size(); ++i)
    {
        SectionComponent* const section = propertyHolderComponent->sections.getUnchecked(i);

        if (section->getName().isNotEmpty())
            s.add (section->getName());
    }

    return s;
}

bool PropertyPanel::isSectionOpen (const int sectionIndex) const
{
    if (SectionComponent* s = propertyHolderComponent->getSectionWithNonEmptyName (sectionIndex))
        return s->isOpen;

    return false;
}

void PropertyPanel::setSectionOpen (const int sectionIndex, const bool shouldBeOpen)
{
    if (SectionComponent* s = propertyHolderComponent->getSectionWithNonEmptyName (sectionIndex))
        s->setOpen (shouldBeOpen);
}

void PropertyPanel::setSectionEnabled (const int sectionIndex, const bool shouldBeEnabled)
{
    if (SectionComponent* s = propertyHolderComponent->getSectionWithNonEmptyName (sectionIndex))
        s->setEnabled (shouldBeEnabled);
}

void PropertyPanel::removeSection (int sectionIndex)
{
    if (SectionComponent* s = propertyHolderComponent->getSectionWithNonEmptyName (sectionIndex))
    {
        propertyHolderComponent->sections.removeObject (s);
        updatePropHolderLayout();
    }
}

//==============================================================================
XmlElement* PropertyPanel::getOpennessState() const
{
    XmlElement* const xml = new XmlElement ("PROPERTYPANELSTATE");

    xml->setAttribute ("scrollPos", viewport.getViewPositionY());

    const StringArray sections (getSectionNames());

    for (int i = 0; i < sections.size(); ++i)
    {
        if (sections[i].isNotEmpty())
        {
            XmlElement* const e = xml->createNewChildElement ("SECTION");
            e->setAttribute ("name", sections[i]);
            e->setAttribute ("open", isSectionOpen (i) ? 1 : 0);
        }
    }

    return xml;
}

void PropertyPanel::restoreOpennessState (const XmlElement& xml)
{
    if (xml.hasTagName ("PROPERTYPANELSTATE"))
    {
        const StringArray sections (getSectionNames());

        forEachXmlChildElementWithTagName (xml, e, "SECTION")
        {
            setSectionOpen (sections.indexOf (e->getStringAttribute ("name")),
                            e->getBoolAttribute ("open"));
        }

        viewport.setViewPosition (viewport.getViewPositionX(),
                                  xml.getIntAttribute ("scrollPos", viewport.getViewPositionY()));
    }
}

//==============================================================================
void PropertyPanel::setMessageWhenEmpty (const String& newMessage)
{
    if (messageWhenEmpty != newMessage)
    {
        messageWhenEmpty = newMessage;
        repaint();
    }
}

const String& PropertyPanel::getMessageWhenEmpty() const noexcept
{
    return messageWhenEmpty;
}

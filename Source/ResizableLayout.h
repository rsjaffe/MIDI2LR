#pragma once
/*******************************************************************************
"A Collection of Useful C++ Classes for Digital Signal Processing"
By Vinnie Falco
Official project location:
https://github.com/vinniefalco/DSPFilters
See Documentation.cpp for contact information, notes, and bibliography.
--------------------------------------------------------------------------------
License: MIT License (http://www.opensource.org/licenses/mit-license.php)
Copyright (c) 2009 by Vinnie Falco
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*******************************************************************************/
#ifndef MIDI2LR_RESIZABLELAYOUT_H_INCLUDED
#define MIDI2LR_RESIZABLELAYOUT_H_INCLUDED
#include "JuceHeader.h"

//==============================================================================
/**
Smoothly repositions and resizes child Components without rounding errors,
according to caller provided stretching parameters.
Usage:
1) Derive your Component from ResizableLayout
2) Give your Component a well defined default size
3) Add child Components and position them according to how
they should appear at the default.
4) For each child Component, also call addToLayout() and specify what
portion of the owner's growth or reduction should apply to each corner.
5) At the end of your constructor, call activateLayout() to turn it on
6) If you later manually reposition a control (for example, using a
ComponentDragger) call updateLayoutFor() on the control that moved,
or if you moved all the controls call updateLayout().
7) You do not need to override resized() to make this work, all you
have to do is lay things out when you create your child Components
and add anchors as appropriate.
Example
struct ContentComponent : Component, ResizableLayout
{
ContentComponent() : ResizableLayout (this)
{
// for layouts to work you must start at some size
// place controls in a location that is initially correct.
setSize(512, 384);
TextButton* b;
b = new TextButton ("Juce");
b->setBounds (32, 32, 512-64, 32);
// stretch to fit the width of the window
addToLayout (b, anchorTopLeft, anchorTopRight);
addAndMakeVisible (b);
b = new TextButton ("One");
b->setBounds (32, 96, 128, 32);
// take up 1/3 of the width of the window
addToLayout (b, Point<int>(0,0), Point<int>(33,0));
addAndMakeVisible (b);
// turn it on
activateLayout();
}
}
*/

class ResizableChild {
public:
    ResizableChild() noexcept :
        m_minW(0), m_maxW(0x3fffffff), m_minH(0), m_maxH(0x3fffffff)
    {}

    virtual ~ResizableChild() = default;//added 6/2016 because there is a virtual member function

    void setMinimumWidth(int minimumWidth) noexcept
    {
        m_minW = minimumWidth;
    }
    void setMaximumWidth(int maximumWidth) noexcept
    {
        m_maxW = maximumWidth;
    }
    void setMinimumHeight(int minimumHeight) noexcept
    {
        m_minH = minimumHeight;
    }
    void setMaximumHeight(int maximumHeight) noexcept
    {
        m_maxH = maximumHeight;
    }

    int getMinimumWidth() const noexcept
    {
        return m_minW;
    }
    int getMaximumWidth() const noexcept
    {
        return m_maxW;
    }
    int getMinimumHeight() const noexcept
    {
        return m_minH;
    }
    int getMaximumHeight() const noexcept
    {
        return m_maxH;
    }

    void setMinimumSize(int minimumWidth, int minimumHeight) noexcept
    {
        setMinimumWidth(minimumWidth);
        setMinimumHeight(minimumHeight);
    }

    void setMaximumSize(int maximumWidth, int maximumHeight) noexcept
    {
        setMaximumWidth(maximumWidth);
        setMaximumHeight(maximumHeight);
    }

    void setSizeLimits(int minimumWidth, int minimumHeight, int maximumWidth,
        int maximumHeight) noexcept
    {
        setMinimumWidth(minimumWidth);
        setMaximumWidth(maximumWidth);
        setMinimumHeight(minimumHeight);
        setMaximumHeight(maximumHeight);
    }

    virtual void resizeStart() noexcept
    {}

private:
    int m_minW;
    int m_maxW;
    int m_minH;
    int m_maxH;
};

class ResizableLayout: public ResizableChild, private juce::ComponentListener {
public:
    enum {
        anchorUnit = 100
    };

    enum Style {
        styleStretch,
        styleFixedAspect
    };



    ResizableLayout(juce::Component* owner);
    ~ResizableLayout();

    // Add a Component to the Layout. topLeft and bottomRight are the percentages
    // that the top left and bottom right of the Component should move by, when
    // the layout is resized. So if you wanted to have the control take on the
    // full width of the parent, and half the height, you would use
    // bottomRight.x=100, bottomRight.y=50. or use the constant anchorMidRight
    void addToLayout(juce::Component *component, const juce::Point<int>& topLeft,
        const juce::Point<int>& bottomRight = juce::Point<int>{-1,-1},
        Style style = styleStretch);
    // Remove a Component from the Layout.
    void removeFromLayout(juce::Component* component) noexcept;
    // Activate (or deactivate) the Layout. The Layout is initially inactive, to
    // prevent spurious recalculation while a Component and its children are
    // being constructed (causing resized() messages). Activating the Layout for
    // the first time will cause an Update().
    void activateLayout(bool isActive = true) noexcept;
    // Call this to manually update the state information for a single control
    // after it has been moved or resized from elsewhere.
    // UNFINISHED API
    void updateLayoutFor(juce::Component *component) noexcept;
    // Convenience function
    static juce::Rectangle<int> calcBoundsOfChildren(juce::Component* parent) noexcept;
    void recalculateLayout() const;

private:
    // Update the state information for all items. This is used on the first
    // Activate(), and can also be used if multiple controls are moved or resized
    // from elsewhere. UNFINISHED API
    void updateLayout() noexcept;
    void resizeStart() noexcept override;

    friend class TopLevelResizableLayout;

    struct Rect {
        Rect() noexcept
        {}
        Rect(int top0, int left0, int bottom0, int right0) noexcept: top{top0},
            left{left0}, bottom{bottom0}, right{right0}
        {}
        Rect(const juce::Rectangle<int>& r) noexcept : top{int(r.getY())},
            left{int(r.getX())}, bottom{int(r.getBottom())},
            right{int(r.getRight())}
        {}
        operator juce::Rectangle<int>() const noexcept
        {
            return juce::Rectangle<int>(left, top, getWidth(), getHeight());
        }
        int getHeight() const noexcept
        {
            return bottom - top;
        }
        int getWidth() const noexcept
        {
            return right - left;
        }
        void reduce(int dx, int dy) noexcept
        {
            top += dy; left += dx; bottom -= dy; right -= dx;
        }

        int top{0};
        int left{0};
        int bottom{0};
        int right{0};
    };

    struct Anchor {
        Style style{Style::styleFixedAspect};
        juce::Component* component;
        ResizableChild* child;
        juce::Point<int> topLeft;
        juce::Point<int> bottomRight;
        Anchor(juce::Component* component = nullptr);
        bool operator== (const Anchor& rhs) const noexcept;
        bool operator>= (const Anchor& rhs) const noexcept;
        bool operator<  (const Anchor& rhs) const noexcept;
    };

    struct State {
        juce::Component* component;
        double aspect{0.0};
        Rect margin;
        State(juce::Component* component = nullptr) noexcept;
        State(const State& other) noexcept;
        State& operator=(const State& other) noexcept;
        bool operator== (const State& rhs) const noexcept;
        bool operator>= (const State& rhs) const noexcept;
        bool operator<  (const State& rhs) const noexcept;
    };

    void addStateFor(const Anchor& anchor) noexcept;
    void componentMovedOrResized(juce::Component& component,
        bool wasMoved, bool wasResized) override;
    void componentBeingDeleted(juce::Component& component) noexcept override;
    juce::Component* m_owner;
    juce::SortedSet<Anchor> m_anchors;
    juce::SortedSet<State> m_states;
    bool m_bFirstTime;
    bool m_isActive;
};

class TopLevelResizableLayout: public ResizableLayout {
public:
    TopLevelResizableLayout(juce::Component* owner);
    void setAsConstrainerFor(juce::ResizableWindow* window);

private:
    class Constrainer: public juce::ComponentBoundsConstrainer {
    public:
        Constrainer(TopLevelResizableLayout* owner) noexcept;
        void resizeStart() noexcept override;
    private:
        TopLevelResizableLayout& m_owner;
    };
    Constrainer m_constrainer;
};

static constexpr juce::Point<int> anchorNone{-1, -1};
static constexpr juce::Point<int> anchorTopLeft{0, 0};
static constexpr juce::Point<int> anchorTopCenter{ResizableLayout::anchorUnit / 2, 0};
static constexpr juce::Point<int> anchorTopRight{ResizableLayout::anchorUnit, 0};
static constexpr juce::Point<int> anchorMidLeft{0, ResizableLayout::anchorUnit / 2};
static constexpr juce::Point<int> anchorMidCenter{ResizableLayout::anchorUnit / 2, ResizableLayout::anchorUnit / 2};
static constexpr juce::Point<int> anchorMidRight{ResizableLayout::anchorUnit, ResizableLayout::anchorUnit / 2};
static constexpr juce::Point<int> anchorBottomLeft{0, ResizableLayout::anchorUnit};
static constexpr juce::Point<int> anchorBottomCenter{ResizableLayout::anchorUnit / 2, ResizableLayout::anchorUnit};
static constexpr juce::Point<int> anchorBottomRight{ResizableLayout::anchorUnit, ResizableLayout::anchorUnit};

#endif
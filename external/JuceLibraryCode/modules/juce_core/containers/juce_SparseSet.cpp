/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

#if JUCE_UNIT_TESTS

class SparseSetTests final : public UnitTest
{
public:
    SparseSetTests()
        : UnitTest ("SparseSet class", UnitTestCategories::containers)
    {}

    void runTest() override
    {
        beginTest ("basic operations");
        {
            SparseSet<int> set;

            expect (set.isEmpty());
            expectEquals (set.size(), 0);
            expectEquals (set.getNumRanges(), 0);
            expect (set.getTotalRange().isEmpty());

            set.addRange ({0, 10});
            expect (! set.isEmpty());
            expectEquals (set.size(), 10);
            expectEquals (set.getNumRanges(), 1);
            expect (! set.getTotalRange().isEmpty());
            expect (set.getRange (0) == Range<int> (0, 10));

            expectEquals (set[0], 0);
            expectEquals (set[5], 5);
            expectEquals (set[9], 9);
            // Index out of range yields a default value for a type
            expectEquals (set[10], 0);
            expect (set.contains (0));
            expect (set.contains (9));
            expect (! set.contains (10));
        }

        beginTest ("adding ranges");
        {
            SparseSet<int> set;

            // Adding same range twice should yield just a single range
            set.addRange ({0, 10});
            set.addRange ({0, 10});
            expectEquals (set.getNumRanges(), 1);
            expect (set.getRange (0) == Range<int> (0, 10));

            // Adding already included range does not increase num ranges
            set.addRange ({0, 2});
            expectEquals (set.getNumRanges(), 1);
            set.addRange ({8, 10});
            expectEquals (set.getNumRanges(), 1);
            set.addRange ({2, 5});
            expectEquals (set.getNumRanges(), 1);

            // Adding non adjacent range includes total number of ranges
            set.addRange ({-10, -5});
            expectEquals (set.getNumRanges(), 2);
            expect (set.getRange (0) == Range<int> (-10, -5));
            expect (set.getRange (1) == Range<int> (0, 10));
            expect (set.getTotalRange() == Range<int> (-10, 10));

            set.addRange ({15, 20});
            expectEquals (set.getNumRanges(), 3);
            expect (set.getRange (0) == Range<int> (-10, -5));
            expect (set.getRange (1) == Range<int> (0, 10));
            expect (set.getRange (2) == Range<int> (15, 20));
            expect (set.getTotalRange() == Range<int> (-10, 20));

            // Adding adjacent ranges merges them.
            set.addRange ({-5, -3});
            expectEquals (set.getNumRanges(), 3);
            expect (set.getRange (0) == Range<int> (-10, -3));
            expect (set.getRange (1) == Range<int> (0, 10));
            expect (set.getRange (2) == Range<int> (15, 20));
            expect (set.getTotalRange() == Range<int> (-10, 20));

            set.addRange ({20, 25});
            expectEquals (set.getNumRanges(), 3);
            expect (set.getRange (0) == Range<int> (-10, -3));
            expect (set.getRange (1) == Range<int> (0, 10));
            expect (set.getRange (2) == Range<int> (15, 25));
            expect (set.getTotalRange() == Range<int> (-10, 25));

            // Adding range containing other ranges merges them
            set.addRange ({-50, 50});
            expectEquals (set.getNumRanges(), 1);
            expect (set.getRange (0) == Range<int> (-50, 50));
            expect (set.getTotalRange() == Range<int> (-50, 50));
        }

        beginTest ("removing ranges");
        {
            SparseSet<int> set;

            set.addRange ({-20, -10});
            set.addRange ({0, 10});
            set.addRange ({20, 30});
            expectEquals (set.getNumRanges(), 3);

            // Removing ranges not included in the set has no effect
            set.removeRange ({-5, 5});
            expectEquals (set.getNumRanges(), 3);

            // Removing partially overlapping range
            set.removeRange ({-15, 5});
            expectEquals (set.getNumRanges(), 3);
            expect (set.getRange (0) == Range<int> (-20, -15));
            expect (set.getRange (1) == Range<int> (5, 10));
            expect (set.getRange (2) == Range<int> (20, 30));

            // Removing subrange of existing range
            set.removeRange ({20, 22});
            expectEquals (set.getNumRanges(), 3);
            expect (set.getRange (2) == Range<int> (22, 30));

            set.removeRange ({28, 30});
            expectEquals (set.getNumRanges(), 3);
            expect (set.getRange (2) == Range<int> (22, 28));

            set.removeRange ({24, 26});
            expectEquals (set.getNumRanges(), 4);
            expect (set.getRange (0) == Range<int> (-20, -15));
            expect (set.getRange (1) == Range<int> (5, 10));
            expect (set.getRange (2) == Range<int> (22, 24));
            expect (set.getRange (3) == Range<int> (26, 28));
        }

        beginTest ("XORing ranges");
        {
            SparseSet<int> set;
            set.addRange ({0, 10});

            set.invertRange ({0, 10});
            expectEquals (set.getNumRanges(), 0);
            set.invertRange ({0, 10});
            expectEquals (set.getNumRanges(), 1);

            set.invertRange ({4, 6});
            expectEquals (set.getNumRanges(), 2);
            expect (set.getRange (0) == Range<int> (0, 4));
            expect (set.getRange (1) == Range<int> (6, 10));

            set.invertRange ({-2, 2});
            expectEquals (set.getNumRanges(), 3);
            expect (set.getRange (0) == Range<int> (-2, 0));
            expect (set.getRange (1) == Range<int> (2, 4));
            expect (set.getRange (2) == Range<int> (6, 10));
        }

        beginTest ("range contains & overlaps checks");
        {
            SparseSet<int> set;
            set.addRange ({0, 10});

            expect (set.containsRange (Range<int> (0, 2)));
            expect (set.containsRange (Range<int> (8, 10)));
            expect (set.containsRange (Range<int> (0, 10)));

            expect (! set.containsRange (Range<int> (-2, 0)));
            expect (! set.containsRange (Range<int> (-2, 10)));
            expect (! set.containsRange (Range<int> (10, 12)));
            expect (! set.containsRange (Range<int> (0, 12)));

            expect (set.overlapsRange (Range<int> (0, 2)));
            expect (set.overlapsRange (Range<int> (8, 10)));
            expect (set.overlapsRange (Range<int> (0, 10)));

            expect (! set.overlapsRange (Range<int> (-2, 0)));
            expect (  set.overlapsRange (Range<int> (-2, 10)));
            expect (! set.overlapsRange (Range<int> (10, 12)));
            expect (  set.overlapsRange (Range<int> (0, 12)));
        }
    }
};

static SparseSetTests sparseSetTests;

#endif

} // namespace juce

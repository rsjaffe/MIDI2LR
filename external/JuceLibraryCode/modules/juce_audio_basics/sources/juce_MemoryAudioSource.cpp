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

MemoryAudioSource::MemoryAudioSource (AudioBuffer<float>& bufferToUse, bool copyMemory, bool shouldLoop)
    : isCurrentlyLooping (shouldLoop)
{
    if (copyMemory)
        buffer.makeCopyOf (bufferToUse);
    else
        buffer.setDataToReferTo (bufferToUse.getArrayOfWritePointers(),
                                 bufferToUse.getNumChannels(),
                                 bufferToUse.getNumSamples());
}

//==============================================================================
void MemoryAudioSource::prepareToPlay (int /*samplesPerBlockExpected*/, double /*sampleRate*/)
{
    position = 0;
}

void MemoryAudioSource::releaseResources()   {}

void MemoryAudioSource::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (buffer.getNumSamples() == 0)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    auto& dst = *bufferToFill.buffer;
    auto channels = jmin (dst.getNumChannels(), buffer.getNumChannels());
    int max = 0, pos = 0;
    auto n = buffer.getNumSamples();
    auto m = bufferToFill.numSamples;

    int i = position;
    for (; (i < n || isCurrentlyLooping) && (pos < m); i += max)
    {
        max = jmin (m - pos, n - (i % n));

        int ch = 0;
        for (; ch < channels; ++ch)
            dst.copyFrom (ch, bufferToFill.startSample + pos, buffer, ch, i % n, max);

        for (; ch < dst.getNumChannels(); ++ch)
            dst.clear (ch, bufferToFill.startSample + pos, max);

        pos += max;
    }

    if (pos < m)
        dst.clear (bufferToFill.startSample + pos, m - pos);

    position = i;
}

//==============================================================================
void MemoryAudioSource::setNextReadPosition (int64 newPosition)
{
    position = (int) newPosition;
}

int64 MemoryAudioSource::getNextReadPosition() const
{
    return position;
}

int64 MemoryAudioSource::getTotalLength() const
{
    return buffer.getNumSamples();
}

//==============================================================================
bool MemoryAudioSource::isLooping() const
{
    return isCurrentlyLooping;
}

void MemoryAudioSource::setLooping (bool shouldLoop)
{
    isCurrentlyLooping = shouldLoop;
}

//==============================================================================
//==============================================================================
#if JUCE_UNIT_TESTS

struct MemoryAudioSourceTests final : public UnitTest
{
    MemoryAudioSourceTests()  : UnitTest ("MemoryAudioSource", UnitTestCategories::audio)  {}

    void runTest() override
    {
        constexpr int blockSize = 512;
        AudioBuffer<float> bufferToFill { 2, blockSize };
        AudioSourceChannelInfo channelInfo { bufferToFill };

        beginTest ("A zero-length buffer produces silence, whether or not looping is enabled");
        {
            for (const bool enableLooping : { false, true })
            {
                AudioBuffer<float> buffer;
                MemoryAudioSource source { buffer, true, false };
                source.setLooping (enableLooping);
                source.prepareToPlay (blockSize, 44100.0);

                for (int i = 0; i < 2; ++i)
                {
                    play (source, channelInfo);
                    expect (isSilent (bufferToFill));
                }
            }
        }

        beginTest ("A short buffer without looping is played once and followed by silence");
        {
            auto buffer = getShortBuffer();
            MemoryAudioSource source { buffer, true, false };
            source.setLooping (false);
            source.prepareToPlay (blockSize, 44100.0);

            play (source, channelInfo);

            auto copy = buffer;
            copy.setSize (buffer.getNumChannels(), blockSize, true, true, false);

            expect (bufferToFill == copy);

            play (source, channelInfo);

            expect (isSilent (bufferToFill));
        }

        beginTest ("A short buffer with looping is played multiple times");
        {
            auto buffer = getShortBuffer();
            MemoryAudioSource source { buffer, true, false };
            source.setLooping (true);
            source.prepareToPlay (blockSize, 44100.0);

            play (source, channelInfo);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                expectEquals (bufferToFill.getSample (0, sample + buffer.getNumSamples()), buffer.getSample (0, sample));

            expect (! isSilent (bufferToFill));
        }

        beginTest ("A long buffer without looping is played once");
        {
            auto buffer = getLongBuffer();
            MemoryAudioSource source { buffer, true, false };
            source.setLooping (false);
            source.prepareToPlay (blockSize, 44100.0);

            play (source, channelInfo);

            auto copy = buffer;
            copy.setSize (buffer.getNumChannels(), blockSize, true, true, false);

            expect (bufferToFill == copy);

            for (int i = 0; i < 10; ++i)
                play (source, channelInfo);

            expect (isSilent (bufferToFill));
        }

        beginTest ("A long buffer with looping is played multiple times");
        {
            auto buffer = getLongBuffer();
            MemoryAudioSource source { buffer, true, false };
            source.setLooping (true);
            source.prepareToPlay (blockSize, 44100.0);

            for (int i = 0; i < 100; ++i)
            {
                play (source, channelInfo);
                expectEquals (bufferToFill.getSample (0, 0), buffer.getSample (0, (i * blockSize) % buffer.getNumSamples()));
            }
        }
    }

    static AudioBuffer<float> getTestBuffer (int length)
    {
        AudioBuffer<float> buffer { 2, length };

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                buffer.setSample (channel, sample, jmap ((float) sample, 0.0f, (float) length, -1.0f, 1.0f));

        return buffer;
    }

    static AudioBuffer<float> getShortBuffer()  { return getTestBuffer (5); }
    static AudioBuffer<float> getLongBuffer()   { return getTestBuffer (1000); }

    static void play (MemoryAudioSource& source, AudioSourceChannelInfo& info)
    {
        info.clearActiveBufferRegion();
        source.getNextAudioBlock (info);
    }

    static bool isSilent (const AudioBuffer<float>& b)
    {
        for (int channel = 0; channel < b.getNumChannels(); ++channel)
            if (b.findMinMax (channel, 0, b.getNumSamples()) != Range<float>{})
                return false;

        return true;
    }
};

static MemoryAudioSourceTests memoryAudioSourceTests;

#endif

} // namespace juce

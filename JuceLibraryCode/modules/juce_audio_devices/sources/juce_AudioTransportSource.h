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

#ifndef JUCE_AUDIOTRANSPORTSOURCE_H_INCLUDED
#define JUCE_AUDIOTRANSPORTSOURCE_H_INCLUDED


//==============================================================================
/**
    An AudioSource that takes a PositionableAudioSource and allows it to be
    played, stopped, started, etc.

    This can also be told use a buffer and background thread to read ahead, and
    if can correct for different sample-rates.

    You may want to use one of these along with an AudioSourcePlayer and AudioIODevice
    to control playback of an audio file.

    @see AudioSource, AudioSourcePlayer
*/
class JUCE_API  AudioTransportSource  : public PositionableAudioSource,
                                        public ChangeBroadcaster
{
public:
    //==============================================================================
    /** Creates an AudioTransportSource.
        After creating one of these, use the setSource() method to select an input source.
    */
    AudioTransportSource();

    /** Destructor. */
    ~AudioTransportSource();

    //==============================================================================
    /** Sets the reader that is being used as the input source.

        This will stop playback, reset the position to 0 and change to the new reader.

        The source passed in will not be deleted by this object, so must be managed by
        the caller.

        @param newSource                        the new input source to use. This may be a nullptr
        @param readAheadBufferSize              a size of buffer to use for reading ahead. If this
                                                is zero, no reading ahead will be done; if it's
                                                greater than zero, a BufferingAudioSource will be used
                                                to do the reading-ahead. If you set a non-zero value here,
                                                you'll also need to set the readAheadThread parameter.
        @param readAheadThread                  if you set readAheadBufferSize to a non-zero value, then
                                                you'll also need to supply this TimeSliceThread object for
                                                the background reader to use. The thread object must not be
                                                deleted while the AudioTransport source is still using it.
        @param sourceSampleRateToCorrectFor     if this is non-zero, it specifies the sample
                                                rate of the source, and playback will be sample-rate
                                                adjusted to maintain playback at the correct pitch. If
                                                this is 0, no sample-rate adjustment will be performed
        @param maxNumChannels                   the maximum number of channels that may need to be played
    */
    void setSource (PositionableAudioSource* newSource,
                    int readAheadBufferSize = 0,
                    TimeSliceThread* readAheadThread = nullptr,
                    double sourceSampleRateToCorrectFor = 0.0,
                    int maxNumChannels = 2);

    //==============================================================================
    /** Changes the current playback position in the source stream.

        The next time the getNextAudioBlock() method is called, this
        is the time from which it'll read data.

        @see getPosition
    */
    void setPosition (double newPosition);

    /** Returns the position that the next data block will be read from
        This is a time in seconds.
    */
    double getCurrentPosition() const;

    /** Returns the stream's length in seconds. */
    double getLengthInSeconds() const;

    /** Returns true if the player has stopped because its input stream ran out of data. */
    bool hasStreamFinished() const noexcept             { return inputStreamEOF; }

    //==============================================================================
    /** Starts playing (if a source has been selected).

        If it starts playing, this will send a message to any ChangeListeners
        that are registered with this object.
    */
    void start();

    /** Stops playing.

        If it's actually playing, this will send a message to any ChangeListeners
        that are registered with this object.
    */
    void stop();

    /** Returns true if it's currently playing. */
    bool isPlaying() const noexcept     { return playing; }

    //==============================================================================
    /** Changes the gain to apply to the output.
        @param newGain  a factor by which to multiply the outgoing samples,
                        so 1.0 = 0dB, 0.5 = -6dB, 2.0 = 6dB, etc.
    */
    void setGain (float newGain) noexcept;

    /** Returns the current gain setting.
        @see setGain
    */
    float getGain() const noexcept      { return gain; }

    //==============================================================================
    /** Implementation of the AudioSource method. */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    /** Implementation of the AudioSource method. */
    void releaseResources() override;

    /** Implementation of the AudioSource method. */
    void getNextAudioBlock (const AudioSourceChannelInfo&) override;

    //==============================================================================
    /** Implements the PositionableAudioSource method. */
    void setNextReadPosition (int64 newPosition) override;

    /** Implements the PositionableAudioSource method. */
    int64 getNextReadPosition() const override;

    /** Implements the PositionableAudioSource method. */
    int64 getTotalLength() const override;

    /** Implements the PositionableAudioSource method. */
    bool isLooping() const override;

private:
    //==============================================================================
    PositionableAudioSource* source;
    ResamplingAudioSource* resamplerSource;
    BufferingAudioSource* bufferingSource;
    PositionableAudioSource* positionableSource;
    AudioSource* masterSource;

    CriticalSection callbackLock;
    float volatile gain, lastGain;
    bool volatile playing, stopped;
    double sampleRate, sourceSampleRate;
    int blockSize, readAheadBufferSize;
    bool volatile isPrepared, inputStreamEOF;

    void releaseMasterResources();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioTransportSource)
};


#endif   // JUCE_AUDIOTRANSPORTSOURCE_H_INCLUDED

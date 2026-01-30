/*#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace mixer
{
    class CrossFader
    {
    public:
        CrossFader();

    
        void setPosition (float newPosition);

        float getPosition() const;

  
        void process (const juce::AudioBuffer<float>& inputA,
                      const juce::AudioBuffer<float>& inputB,
                      juce::AudioBuffer<float>& output) const;

    private:
        float position = 0.0f;
    };
}
    static bool loadAudioFile (const juce::File& file,
                           juce::AudioBuffer<float>& buffer,
                           double& sampleRate)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats(); // WAV, AIFF, MP3, FLAC, etc.

    std::unique_ptr<juce::AudioFormatReader> reader (
        formatManager.createReaderFor (file));

    if (reader == nullptr)
        return false;

    sampleRate = reader->sampleRate;

    buffer.setSize ((int) reader->numChannels,
                    (int) reader->lengthInSamples);

    reader->read (&buffer,
                  0,
                  (int) reader->lengthInSamples,
                  0,
                  true,
                  true);

    return true;
}

*/



#pragma once


#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace mixer
{
    class CrossFader
    {
    public:
        CrossFader();

        /** Set crossfade position.
            0.0 = full A
            1.0 = full B
        */
        void setPosition (float newPosition);

        float getPosition() const;

        /** Crossfade two input buffers into the output buffer.
            All buffers must have the same number of channels & samples.
        */
        void process (const juce::AudioBuffer<float>& inputA,
                      const juce::AudioBuffer<float>& inputB,
                      juce::AudioBuffer<float>& output) const;

    private:
        float position = 0.0f;
    };
}

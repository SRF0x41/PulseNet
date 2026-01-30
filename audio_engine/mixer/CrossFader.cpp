#include "CrossFader.h"

namespace mixer
{
    CrossFader::CrossFader() = default;

    void CrossFader::setPosition (float newPosition)
    {
        position = juce::jlimit (0.0f, 1.0f, newPosition);
    }

    float CrossFader::getPosition() const
    {
        return position;
    }

    void CrossFader::process (const juce::AudioBuffer<float>& inputA,
                              const juce::AudioBuffer<float>& inputB,
                              juce::AudioBuffer<float>& output) const
    {
        jassert (inputA.getNumChannels() == inputB.getNumChannels());
        jassert (inputA.getNumSamples()  == inputB.getNumSamples());

        output.setSize (inputA.getNumChannels(),
                        inputA.getNumSamples(),
                        false, false, true);

        const float gainA = 1.0f - position;
        const float gainB = position;

        for (int ch = 0; ch < output.getNumChannels(); ++ch)
        {
            const float* a = inputA.getReadPointer (ch);
            const float* b = inputB.getReadPointer (ch);
            float* out     = output.getWritePointer (ch);

            for (int i = 0; i < output.getNumSamples(); ++i)
                out[i] = (a[i] * gainA) + (b[i] * gainB);
        }
    }
}

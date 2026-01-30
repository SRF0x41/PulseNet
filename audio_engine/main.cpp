#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "mixer/CrossFader.h"



int main()
{
    juce::ConsoleApplication app;

    constexpr int channels = 2;
    constexpr int samples  = 512;

    juce::AudioBuffer<float> a (channels, samples);
    juce::AudioBuffer<float> b (channels, samples);
    juce::AudioBuffer<float> out;

    a.clear();
    b.clear();

    // Fill buffers with dummy signals
    a.applyGain (0.25f);
    b.applyGain (1.0f);

    mixer::CrossFader xf;
    xf.setPosition (0.5f); // equal blend

    xf.process (a, b, out);

    juce::Logger::writeToLog ("Crossfade processed successfully");

    return 0;
}

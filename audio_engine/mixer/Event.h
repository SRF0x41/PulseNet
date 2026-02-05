#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <string>
class Event{
    public:
    Event(const std::string eventType);
    ~Event();
    private:

    std::string eventType;
    juce::AudioTransportSource* track;


}
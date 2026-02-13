cmake_minimum_required(VERSION 3.15)
project(PulseNet VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)  # JUCE repo cloned here





juce_add_console_app(PulseNet
    PRODUCT_NAME "PulseNet"
)


target_compile_definitions(PulseNet
   PUBLIC JUCE_USE_MP3AUDIOFORMAT=1)


target_sources(PulseNet
    PRIVATE
        main.cpp
        
       
        stream/Streamer.h
        stream/Streamer.cpp
        stream/AudioTrack.h
        stream/AudioTrack.cpp
        stream/EventTimeline.h
        stream/EventTimeline.cpp
        comms/SocketManager.h
        comms/SocketManager.cpp        
)

target_link_libraries(PulseNet
    PRIVATE
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_audio_formats
        juce::juce_audio_devices
        curl
)

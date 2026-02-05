#include "EventTimeline.h"
#include "AudioTrack.h"

EventTimeline::EventTimeline(double sampleRate) {
    currentSampleRate = sampleRate;
}

EventTimeline::~EventTimeline() {}

bool EventTimeline::addTrack(AudioTrack &track) {
  if (!track.getTransport())
    return false;
  TimelineEvent new_track;
  new_track.track = &track;
  new_track.durationSamples = track.getTotalSamples(currentSampleRate);
  if (sumTrackSamples <= 0) {
    new_track.startSample = 0;
  } else {
    new_track.startSample =
        sumTrackSamples - track.getOverlapSamples(currentSampleRate);
  }

  timelineEvents.push_back(new_track);

  sumTrackSamples += track.getTotalSamples(currentSampleRate) -
                     track.getOverlapSamples(currentSampleRate);

  std::sort(timelineEvents.begin(), timelineEvents.end(),
            [](auto &a, auto &b) { return a.startSample < b.startSample; });

  std::cout << "Queued: " << track.getSource() << " at "
            << new_track.startSample << '\n';

  return true;
}


juce::AudioTransportSource* EventTimeline::getEvent(){

}
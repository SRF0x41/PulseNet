#include "EventTimeline.h"
#include "AudioTrack.h"
#include "TimelineEvent.h"
#include <cstddef>

EventTimeline::EventTimeline(double sampleRate) {
  currentSampleRate = sampleRate;
}

EventTimeline::~EventTimeline() {}
/*#pragma once
#include "AudioTrack.h"
#include <cstdint>
struct TimelineEvent {

    int64_t eventSample; // When this event triggers
    AudioTrack *track;   // Which track
    enum EventType { START, STOP, FADE_IN, FADE_OUT } type;

  };*/
bool EventTimeline::addTrack(AudioTrack &track) {
  if (!track.getTransport())
    return false;

  // --- Start event ---
  TimelineEvent START_track;
  START_track.type = TimelineEvent::START;
  START_track.track = &track;
  if (sumTrackSamples <= 0) {
    START_track.eventSample = 0;
  } else {
    START_track.eventSample =
        sumTrackSamples - track.getOverlapSamples(currentSampleRate);
  }
  timeline.push_back(START_track);

  // Update sum of track samples

  sumTrackSamples += track.getTotalSamples(currentSampleRate) -
                     track.getOverlapSamples(currentSampleRate);

  // --- End event ---
  TimelineEvent STOP_track;
  STOP_track.type = TimelineEvent::STOP;
  STOP_track.track = &track;

  int64_t remainingVirtualsamples =
      track.getVirtualRemainingSamples(currentSampleRate);
  if (remainingVirtualsamples > 0) {
    STOP_track.eventSample = sumTrackSamples -
                             track.getTotalSamples(currentSampleRate) +
                             remainingVirtualsamples;
  } else {
    STOP_track.eventSample = sumTrackSamples;
  }
  timeline.push_back(STOP_track);

  // --- Fade in event ---
  if (track.getFadeInDuration_samples() > 0) {
    TimelineEvent FADE_IN_track;
    FADE_IN_track.type = TimelineEvent::FADE_IN;
    FADE_IN_track.eventSample = START_track.eventSample;
    FADE_IN_track.track = &track;
    timeline.push_back(FADE_IN_track);
  }

  // --- Fade out event ---
  if (track.getFadeOutDuration_samples() > 0) {
    TimelineEvent FADE_OUT_event;
    FADE_OUT_event.type = TimelineEvent::FADE_OUT;
    FADE_OUT_event.eventSample =
        STOP_track.eventSample - track.getFadeOutDuration_samples();
    FADE_OUT_event.track = &track;
    timeline.push_back(FADE_OUT_event);
  }

  // Sort timeline by eventSample
  std::sort(timeline.begin(), timeline.end(),
            [](const TimelineEvent &a, const TimelineEvent &b) {
              return a.eventSample < b.eventSample;
            });

  // --- Print all events ---
  std::cout << "Timeline after adding track: " << track.getSource() << "\n";
  for (const auto &ev : timeline) {
    std::string typeStr;
    switch (ev.type) {
    case TimelineEvent::START:
      typeStr = "START";
      break;
    case TimelineEvent::STOP:
      typeStr = "STOP";
      break;
    case TimelineEvent::FADE_IN:
      typeStr = "FADE_IN";
      break;
    case TimelineEvent::FADE_OUT:
      typeStr = "FADE_OUT";
      break;
    default:
      typeStr = "UNKNOWN";
      break;
    }

    std::cout << "Event: " << typeStr
              << " | Track: " << (ev.track ? ev.track->getSource() : "null")
              << " | Sample: " << ev.eventSample << "\n";
  }

  return true;
}

void EventTimeline::startFade(TimelineEvent *event) {
  FadeState push_fade;
  push_fade.fadeStatus = true;
  push_fade.track = event->track;

  if (event->type == TimelineEvent::FADE_IN) {
    push_fade.fadeType = FadeState::FADE_IN;
    push_fade.fadeSamplesRemaining = event->track->getFadeInDuration_samples();
    push_fade.fadeRate = event->track->getFadeInGainRate();
  }
  if (event->type == TimelineEvent::FADE_OUT) {
    push_fade.fadeType = FadeState::FADE_OUT;
    push_fade.fadeSamplesRemaining = event->track->getFadeOutDuration_samples();
    push_fade.fadeRate = event->track->getFadeOutGainRate();
  }
  fadeTimeline.push_back(push_fade);
}

int64_t EventTimeline::advanceNextTrack(){
  while(timeline[eventIndex].type != TimelineEvent::START){
    timeline[eventIndex].eventTriggered = true;
    eventIndex++;
  }
  return timeline[eventIndex].eventSample - 512;
}


TimelineEvent *EventTimeline::getEvent(int64_t endBlock) {
  if (eventIndex >= timeline.size()) {
    return nullptr; // no more events
  }

  if (timeline[eventIndex].eventSample >= endBlock) {
    return nullptr; // event is beyond this block
  }

  // Event is valid for this block
  ++eventIndex;
  return &timeline[eventIndex - 1];
}

/*#pragma once
#include "AudioTrack.h"
#include <cstdint>
struct TimelineEvent {

    int64_t eventSample; // When this event triggers
    AudioTrack *track;   // Which track
    enum EventType { START, STOP, FADE_IN, FADE_OUT } type;

  };*/


// void EventTimeline::updateEventTimeline() {
//   if (eventIndex >= timeline.size())
//     return;

//   // Recompute sumtrack samples up the the last triggered stop

//   for(size_t i = eventIndex; i > 0; i--){
//     if(timeline[i].type == TimelineEvent::STOP){
//       sumTrackSamples = timeline[i].eventSample;
//     }
//   }

//   // Recompute future events only
//   for (size_t i = eventIndex; i < timeline.size(); ++i) {
//     TimelineEvent &ev = timeline[i];
//     AudioTrack *track = ev.track;

//     if (!track)
//       continue;

//     switch (ev.type) {
//     case TimelineEvent::START: {
//       if (i == 0) {
//         ev.eventSample = 0;
//       } else {
//         ev.eventSample =
//             sumTrackSamples - track->getOverlapSamples(currentSampleRate);
//       }
//       break;
//     }

//     case TimelineEvent::STOP: {
//       int64_t remainingVirtualSamples =
//           track->getVirtualRemainingSamples(currentSampleRate);

//       if (remainingVirtualSamples > 0) {
//         ev.eventSample =
//             sumTrackSamples - track->getTotalSamples(currentSampleRate) +
//             remainingVirtualSamples;
//       } else {
//         ev.eventSample = sumTrackSamples;
//       }
//       break;
//     }

//     case TimelineEvent::FADE_IN: {
//       ev.eventSample =
//           track->getStartSample(currentSampleRate);
//       break;
//     }

//     case TimelineEvent::FADE_OUT: {
//       ev.eventSample =
//           track->getStopSample(currentSampleRate) -
//           track->getFadeOutDuration_samples();
//       break;
//     }
//     }
//   }

//   // Re-sort only future events
//   std::sort(timeline.begin() + eventIndex, timeline.end(),
//             [](const TimelineEvent &a, const TimelineEvent &b) {
//               return a.eventSample < b.eventSample;
//             });
// }




// bool EventTimeline::addTrack(AudioTrack &track) {
//   if (!track.getTransport())
//     return false;

//   // --- Start event ---
//   TimelineEvent START_track;
//   START_track.type = TimelineEvent::START;
//   START_track.track = &track;
//   if (sumTrackSamples <= 0) {
//     START_track.eventSample = 0;
//   } else {
//     START_track.eventSample =
//         sumTrackSamples - track.getOverlapSamples(currentSampleRate);
//   }
//   timeline.push_back(START_track);

//   // Update sum of track samples

//   sumTrackSamples += track.getTotalSamples(currentSampleRate) -
//                      track.getOverlapSamples(currentSampleRate);

//   // --- End event ---
//   TimelineEvent STOP_track;
//   STOP_track.type = TimelineEvent::STOP;
//   STOP_track.track = &track;

//   int64_t remainingVirtualsamples =
//       track.getVirtualRemainingSamples(currentSampleRate);
//   if (remainingVirtualsamples > 0) {
//     STOP_track.eventSample = sumTrackSamples -
//                              track.getTotalSamples(currentSampleRate) +
//                              remainingVirtualsamples;
//   } else {
//     STOP_track.eventSample = sumTrackSamples;
//   }
//   timeline.push_back(STOP_track);

//   // --- Fade in event ---
//   if (track.getFadeInDuration_samples() > 0) {
//     TimelineEvent FADE_IN_track;
//     FADE_IN_track.type = TimelineEvent::FADE_IN;
//     FADE_IN_track.eventSample = START_track.eventSample;
//     FADE_IN_track.track = &track;
//     timeline.push_back(FADE_IN_track);
//   }

//   // --- Fade out event ---
//   if (track.getFadeOutDuration_samples() > 0) {
//     TimelineEvent FADE_OUT_event;
//     FADE_OUT_event.type = TimelineEvent::FADE_OUT;
//     FADE_OUT_event.eventSample =
//         STOP_track.eventSample - track.getFadeOutDuration_samples();
//     FADE_OUT_event.track = &track;
//     timeline.push_back(FADE_OUT_event);
//   }

//   // Sort timeline by eventSample
//   std::sort(timeline.begin(), timeline.end(),
//             [](const TimelineEvent &a, const TimelineEvent &b) {
//               return a.eventSample < b.eventSample;
//             });

//   // --- Print all events ---
//   std::cout << "Timeline after adding track: " << track.getSource() << "\n";
//   for (const auto &ev : timeline) {
//     std::string typeStr;
//     switch (ev.type) {
//     case TimelineEvent::START:
//       typeStr = "START";
//       break;
//     case TimelineEvent::STOP:
//       typeStr = "STOP";
//       break;
//     case TimelineEvent::FADE_IN:
//       typeStr = "FADE_IN";
//       break;
//     case TimelineEvent::FADE_OUT:
//       typeStr = "FADE_OUT";
//       break;
//     default:
//       typeStr = "UNKNOWN";
//       break;
//     }

//     std::cout << "Event: " << typeStr
//               << " | Track: " << (ev.track ? ev.track->getSource() : "null")
//               << " | Sample: " << ev.eventSample << "\n";
//   }

//   return true;
// }
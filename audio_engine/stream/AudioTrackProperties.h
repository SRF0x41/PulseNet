#include <string>
struct AudioTrackProperties {
  std::string path;
  double advanceStart;
  double fadeinDuration = 0;
  double virtualEndTrim = 0;
  double fadeOutDuration = 0;
  double overlapDuration = 0;
};
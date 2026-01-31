#include <string>

class AudioTrack {
public:
    AudioTrack(const std::string &sourcePath)
        : source(sourcePath), bpm(0.0f), remainingTime(0.0f)
    {}

    // Accessors
    const std::string& getSource() const { return source; }
    float getBpm() const { return bpm; }
    void setBpm(float newBpm) { bpm = newBpm; }
    float getRemainingTime() const { return remainingTime; }
    void setRemainingTime(float time) { remainingTime = time; }

private:
    const std::string source;  // File path
    float bpm;
    float remainingTime;
};

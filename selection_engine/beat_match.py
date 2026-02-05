"""
Beat Match Two Songs (Last N Bars)

This version aligns song 2 to the last N bars of song 1.
Outputs: seconds BEFORE song 1 ends to start song 2.

Requires:
    librosa, numpy, scipy
"""

import numpy as np
import librosa
from scipy.signal import correlate

# -------------------------
# Helper Functions
# -------------------------

def create_beat_signal(beat_times, duration, sr):
    length = int(duration * sr)
    signal = np.zeros(length, dtype=np.float32)
    for t in beat_times:
        idx = int(t * sr)
        if 0 <= idx < length:
            signal[idx] = 1.0
    return signal

# -------------------------
# Beat Matching (Last N Bars)
# -------------------------
def beat_match_last_n_bars(song1_path, song2_path, bars=8, beats_per_bar=4):
    y1, sr1 = librosa.load(song1_path, mono=True)
    y2, sr2 = librosa.load(song2_path, mono=True)

    # Beat tracking
    _, beats1 = librosa.beat.beat_track(y=y1, sr=sr1)
    _, beats2 = librosa.beat.beat_track(y=y2, sr=sr2)

    times1 = librosa.frames_to_time(beats1, sr=sr1)
    times2 = librosa.frames_to_time(beats2, sr=sr2)

    duration1 = librosa.get_duration(y=y1, sr=sr1)
    duration2 = librosa.get_duration(y=y2, sr=sr2)

    # Select last N bars
    beats_needed = bars * beats_per_bar
    if len(times1) < beats_needed:
        last_beats1 = times1
    else:
        last_beats1 = times1[-beats_needed:]

    last_n_bar_start_time = last_beats1[0]  # first beat of last N bars

    sig1 = create_beat_signal(last_beats1, duration1, sr1)
    sig2 = create_beat_signal(times2, duration2, sr2)

    # Cross-correlation
    correlation = correlate(sig1, sig2, mode="full")
    best_lag = np.argmax(correlation) - len(sig2)
    start_time_seconds = best_lag / sr1

    # 🔹 Constrain start_time to be inside the last N bars
    if start_time_seconds < last_n_bar_start_time:
        start_time_seconds = last_n_bar_start_time

    seconds_before_end = duration1 - start_time_seconds
    return seconds_before_end

# -------------------------
# Entry Point
# -------------------------

if __name__ == "__main__":

    song1 = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Disclosure - Latch ft. Sam Smith.mp3"
    song2 = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/This Summer (JBAG Remix).mp3"

    # Align to last 8 bars
    seconds_before_end = beat_match_last_n_bars(song1, song2, bars=8, beats_per_bar=4)

    print(f"Start song 2 {seconds_before_end:.3f} seconds before song 1 ends")

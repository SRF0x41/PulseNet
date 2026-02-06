"""
Beat Matcher (Last N Bars)

This class aligns song 2 to the last N bars of song 1.
Outputs: seconds BEFORE song 1 ends to start song 2.

Requires:
    librosa, numpy, scipy
"""

import numpy as np
import librosa
from scipy.signal import correlate


class BeatMatcher:
    def __init__(self, bars=8, beats_per_bar=4):
        """
        Initialize the BeatMatcher.

        :param bars: Number of bars at the end of song1 to match
        :param beats_per_bar: Number of beats per bar
        """
        self.bars = bars
        self.beats_per_bar = beats_per_bar

    # -------------------------
    # Helper Functions
    # -------------------------
    @staticmethod
    def create_beat_signal(beat_times, duration, sr):
        """
        Create a spike signal at each beat time.

        :param beat_times: Array of beat times in seconds
        :param duration: Duration of the audio in seconds
        :param sr: Sample rate
        :return: numpy array of shape (duration*sr,)
        """
        length = int(duration * sr)
        signal = np.zeros(length, dtype=np.float32)
        for t in beat_times:
            idx = int(t * sr)
            if 0 <= idx < length:
                signal[idx] = 1.0
        return signal

    # -------------------------
    # Beat Matching Method
    # -------------------------
    def match_last_n_bars(self, song1_path, song2_path):
        """
        Align song2 to the last N bars of song1.

        :param song1_path: Path to first song (reference)
        :param song2_path: Path to second song (to align)
        :return: seconds BEFORE the end of song1 to start song2
        """
        # Load audio
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
        beats_needed = self.bars * self.beats_per_bar
        if len(times1) < beats_needed:
            last_beats1 = times1
        else:
            last_beats1 = times1[-beats_needed:]

        last_n_bar_start_time = last_beats1[0]  # first beat of last N bars

        # Create beat spike signals
        sig1 = self.create_beat_signal(last_beats1, duration1, sr1)
        sig2 = self.create_beat_signal(times2, duration2, sr2)

        # Cross-correlation to find alignment
        correlation = correlate(sig1, sig2, mode="full")
        best_lag = np.argmax(correlation) - len(sig2)
        start_time_seconds = best_lag / sr1

        # Constrain start_time inside the last N bars
        if start_time_seconds < last_n_bar_start_time:
            start_time_seconds = last_n_bar_start_time

        seconds_before_end = duration1 - start_time_seconds
        return seconds_before_end


"""if __name__ == "__main__":
    matcher = BeatMatcher(bars=8, beats_per_bar=4)
    seconds_before_end = matcher.match_last_n_bars("song1.wav", "song2.wav")
    print(f"Start song2 {seconds_before_end:.2f} seconds before song1 ends")
"""

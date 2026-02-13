import librosa
import numpy as np
from audio_track import AudioTrack
from beat_match import BeatMatcher
from advance_song import AdvanceSong

class BatchQueue:
    def __init__(self, sample_track_list, bars=8, beats_per_bar=4):
        self.sample_batch_list = sample_track_list

        self.beat_matcher = BeatMatcher(bars=bars, beats_per_bar=beats_per_bar)

        # Store info for next track
        self.next_track_overlap = 0.0
        self.next_track_advanced_start = 0.0

    # --------------------------
    # Helper: compute fade durations from beat times
    # --------------------------
    @staticmethod
    def fade_durations_from_phrases(beat_times, beats_per_phrase=16, min_fade=2.0, max_fade=5.0):
        if len(beat_times) < beats_per_phrase:
            return min_fade, min_fade

        # Fade-in = duration of first phrase
        fade_in = beat_times[beats_per_phrase - 1] - beat_times[0]
        # Fade-out = duration of last phrase
        fade_out = beat_times[-1] - beat_times[-beats_per_phrase]

        # Clamp values
        fade_in = np.clip(fade_in, min_fade, max_fade)
        fade_out = np.clip(fade_out, min_fade, max_fade)

        return fade_in, fade_out

    # --------------------------
    # Get next track with dynamic start, fade, and overlap
    # --------------------------
    def getNext(self):
        if not self.sample_batch_list:
            return None

        # Last track in queue → no next
        if len(self.sample_batch_list) == 1:
            track_path = self.sample_batch_list.pop(0)
            return AudioTrack(
                track_path,
                advance_start=0.0,
                set_fade_in_duration=4.0,
                set_virtual_end_trim=0.0,
                set_fade_out_duration=4.0,
                set_overlap_duration=self.next_track_overlap
            )

        # Pop current track, peek next
        first_track = self.sample_batch_list.pop(0)
        next_track = self.sample_batch_list[0]

        # --------------------------
        # 1. Determine advanced start of next track
        # --------------------------
        advanced_start = AdvanceSong.advance_to_first_phrase(next_track)

        # --------------------------
        # 2. Determine beat-match offset (seconds before end of first track)
        # --------------------------
        seconds_offset = self.beat_matcher.match_last_n_bars_advanced_second_track(
            first_track,
            next_track,
            advanced_start
        )

        # --------------------------
        # 3. Determine fade-in/out durations dynamically
        # --------------------------
        # Beat analysis of next track
        y2, sr2 = librosa.load(next_track, mono=True)
        _, beats2 = librosa.beat.beat_track(y=y2, sr=sr2)
        beat_times2 = librosa.frames_to_time(beats2, sr=sr2)

        fade_in, fade_out = self.fade_durations_from_phrases(beat_times2)

        # --------------------------
        # 4. Construct AudioTrack
        # --------------------------
        track = AudioTrack(
            track_path=first_track,
            advance_start=self.next_track_advanced_start,
            set_fade_in_duration=fade_in,
            set_virtual_end_trim=max(seconds_offset - fade_out, 0.0),
            set_fade_out_duration=fade_out,
            set_overlap_duration=self.next_track_overlap
        )

        # --------------------------
        # 5. Update state for next iteration
        # --------------------------
        self.next_track_overlap = seconds_offset
        self.next_track_advanced_start = advanced_start

        return track

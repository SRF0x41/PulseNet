from audio_track import AudioTrack
from beat_match import BeatMatcher


class BatchQueue:
    def __init__(self, sample_track_list):
        self.sample_batch_list = sample_track_list

        self.FIXED_ADVANCE_START = 0
        self.FIXED_FADE_IN_DURATION = 5
        self.FIXED_FADE_OUT_DURATION = 5

        # Beat match class
        self.beat_matcher = BeatMatcher()

        # Store a value to the next songs overlap
        self.next_track_overlap = 0.0

        """class AudioTrack:
    def __init__(
        self,
        track_path: str,
        advance_start: float = 0.0,
        set_fade_in_duration: float = 0.0,
        set_virtual_end_trim: float = 0.0,
        set_fade_out_duration: float = 0.0,
        set_overlap_duration: float = 0.0,
    ):"""

    def getNext(self):
        if len(self.sample_batch_list) == 0:
            return None

        if len(self.sample_batch_list) == 1:
            return AudioTrack(
                self.sample_batch_list.pop(0),
                self.FIXED_ADVANCE_START,
                self.FIXED_FADE_IN_DURATION,
                0,
                self.FIXED_FADE_OUT_DURATION,
                self.next_track_overlap,
            )

        first_track = self.sample_batch_list.pop(0)
        next_track = self.sample_batch_list[0]
        seconds_offset = self.beat_matcher.match_last_n_bars(first_track, next_track)

        track = AudioTrack(
            first_track,
            self.FIXED_ADVANCE_START,
            self.FIXED_FADE_IN_DURATION,
            seconds_offset - self.FIXED_FADE_OUT_DURATION - 3,
            self.FIXED_FADE_OUT_DURATION,
            self.next_track_overlap,
        )

        self.next_track_overlap = seconds_offset

        return track

class AudioTrack:
    def __init__(
        self,
        track_path: str,
        advance_start: float = 0.0,
        set_fade_in_duration: float = 0.0,
        set_virtual_end_trim: float = 0.0,
        set_fade_out_duration: float = 0.0,
        set_overlap_duration: float = 0.0,
    ):
        self.track_path = track_path
        self.advance_start = advance_start
        self.set_fade_in_duration = set_fade_in_duration
        self.set_virtual_end_trim = set_virtual_end_trim
        self.set_fade_out_duration = set_fade_out_duration
        self.set_overlap_duration = set_overlap_duration


def to_add_next_json(self) -> dict:
    return {
        "command": "ADD_NEXT",
        "track_path": self.track_path,
        "properties": {
            "advance_start": self.advance_start,
            "set_fade_in_duration": self.set_fade_in_duration,
            "set_virtual_end_trim": self.set_virtual_end_trim,
            "set_fade_out_duration": self.set_fade_out_duration,
            "set_overlap_duration": self.set_overlap_duration,
        },
    }

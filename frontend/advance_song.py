import librosa
import numpy as np

class AdvanceSong:
    @staticmethod
    def advance_to_first_phrase(mp3_path: str, phrase_length_beats: int = 16) -> float:
        y, sr = librosa.load(mp3_path, sr=None)
        
        tempo, beat_frames = librosa.beat.beat_track(y=y, sr=sr)
        tempo = float(tempo) if np.isscalar(tempo) else float(tempo[0])  # fix
        
        beat_times = librosa.frames_to_time(beat_frames, sr=sr)
        
        if len(beat_times) >= phrase_length_beats:
            first_phrase_start = beat_times[phrase_length_beats - 1]
        else:
            first_phrase_start = beat_times[0]
        
        print(f"Detected tempo: {tempo:.2f} BPM")
        print(f"Advancing to first phrase at {first_phrase_start:.2f} seconds")
        
        return first_phrase_start

import librosa

class AdvanceSong:
    def __init__(self):
        pass

def advance_to_first_phrase(mp3_path, phrase_length_beats=16):
    """
    Load an MP3 file and return the timestamp (in seconds) of the start of the first phrase.

    Args:
        mp3_path (str): Path to the MP3 file.
        phrase_length_beats (int): Number of beats per phrase (default 16).

    Returns:
        float: Timestamp in seconds where the first phrase starts.
    """
    # Load the audio
    y, sr = librosa.load(mp3_path, sr=None)
    
    # Detect tempo and beats
    tempo, beat_frames = librosa.beat.beat_track(y=y, sr=sr)
    
    # Convert beat frames to timestamps in seconds
    beat_times = librosa.frames_to_time(beat_frames, sr=sr)
    
    # Start of first phrase is at the first beat that is a multiple of phrase_length_beats
    if len(beat_times) >= phrase_length_beats:
        first_phrase_start = beat_times[phrase_length_beats-1]
    else:
        # If track is short, just return the first beat
        first_phrase_start = beat_times[0]
    
    print(f"Detected tempo: {tempo:.2f} BPM")
    print(f"Advancing to first phrase at {first_phrase_start:.2f} seconds")
    
    return first_phrase_start

import librosa, os

def detect_bpm(audio_path):
    """
    Detect the BPM (tempo) of an audio file.

    Args:
        audio_path (str): Path to the audio file (mp3, wav, etc.)

    Returns:
        float: Estimated BPM of the audio
    """
    # Load the audio file
    y, sr = librosa.load(audio_path, sr=None)  # sr=None preserves original sample rate

    # Estimate tempo (BPM)
    tempo, _ = librosa.beat.beat_track(y=y, sr=sr)

    return tempo

# Example usage
if __name__ == "__main__":


    # Path to your tracks directory (relative to bpm_detector.py)
    tracks_dir = os.path.join(os.path.dirname(__file__), "../get_audio/tracks")

    # List all mp3 files in the directory
    track_files = [f for f in os.listdir(tracks_dir) if f.endswith(".mp3")]

    for track in track_files:
        track_path = os.path.join(tracks_dir, track)
        print("Processing:", track_path)
        # Here you would call your BPM detection function
        bpm = detect_bpm(track_path)
        print(f"Estimated BPM: {bpm}")

    
    

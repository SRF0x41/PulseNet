import json
import os
from playlist import Playlist
from pathlib import Path
from llm_local_api import LLMPlaylistGenerator


def load_config(path="config.json"):
    with open(path, "r") as f:
        return json.load(f)
    

def get_tracks(track_path):
    """Return a sorted list of all mp3 file paths in track_path."""
    path = Path(track_path)

    if not path.exists():
        return []

    tracks = [
        str(item.resolve())
        for item in path.iterdir()
        if item.is_file() and item.suffix.lower() == ".mp3"
    ]

    return sorted(tracks, key=lambda x: x.lower())


def main():
    CONFIG = load_config()
    MASTER_PLAYLIST_PATH = CONFIG.get("track_list_path")
    
    print("--- PulseNet ---")
    print(f"Master playlist path {MASTER_PLAYLIST_PATH}") 
    
    # Get master playlist
    MASTER_PLAYLIST_OBJECT = Playlist(get_tracks(MASTER_PLAYLIST_PATH),"MASTER PLAYLIST")
    
    print(MASTER_PLAYLIST_OBJECT)
    
    llm_recomended_playlist_raw = LLMPlaylistGenerator.generate(MASTER_PLAYLIST_OBJECT.get_all_tracks_name())
    
    # Validate that every element of the raw list is a real track
    validated_tracks_by_name = []
    for output in llm_recomended_playlist_raw:
        if MASTER_PLAYLIST_OBJECT.exists_by_name(output):
            validated_tracks_by_name.append(output)
    print(f"{len(validated_tracks_by_name)} tracks validated out of {MASTER_PLAYLIST_OBJECT.get_size()}")
    
    
    # Send the playlist to the scheduler
    
    
    # Await client connection 
    socket = SocketServer()
    socket.start()
    
    
    
    


if __name__ == "__main__":
    main()

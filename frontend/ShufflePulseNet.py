import json
import os
from playlist import Playlist
from pathlib import Path
from llm_local_api import LLMPlaylistGenerator
from batch_queue_ver3 import BatchQueue
from socket_server import SocketServer


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

def get_default_playlist_paths(root,playlist):
    out = []
    for track in playlist:
        out.append(root+"/"+track)
        
    return out
    


def main():
    CONFIG = load_config()
    MASTER_PLAYLIST_PATH = CONFIG.get("track_list_root_path")
    
    print("--- PulseNet ---")
    print(f"Master playlist path {MASTER_PLAYLIST_PATH}") 
    
    # Get master playlist
    # MASTER_PLAYLIST_OBJECT = Playlist(get_tracks(MASTER_PLAYLIST_PATH),"MASTER PLAYLIST")
    
    MASTER_PLAYLIST_OBJECT = Playlist(get_default_playlist_paths(MASTER_PLAYLIST_PATH,CONFIG.get("default_playlist")))
    
    print("--- Loaded Master Playlist ---")
    print(MASTER_PLAYLIST_OBJECT)
    
    print("--- Shuffle Master Playlist ---")
    MASTER_PLAYLIST_OBJECT.shuffle_order()
    print(MASTER_PLAYLIST_OBJECT)
    
    # Await client connection 
    socket = SocketServer()
    socket.start()
    
     # Show remaining tracks before any processing
    batch_queue = BatchQueue(MASTER_PLAYLIST_OBJECT.get_all_tracks_path())
    
    next_track = batch_queue.getNext()
    while next_track:
        print(next_track)
        track_json = json.dumps(next_track.to_json())
        socket.send(track_json.encode('utf-8'))
        next_track = batch_queue.getNext()
        
        
    
    
        
        
    
    
    # Send the playlist to the scheduler
    valid_commands = ["pause", "resume", "skip", "quit"]

    while True:
        cmd = input("Command: ").strip().lower()
        
        if cmd == "quit":
            break
        
        if cmd in valid_commands:
            print(f"Executing {cmd}...")
        else:
            print("Unknown command. Valid commands:", valid_commands)
            
        if cmd == "pause":
            pause_command = {"command":"PAUSE"}
            socket.send(json.dumps(pause_command).encode('utf-8'))
        
        if cmd == "resume":
            resume_command = {"command":"RESUME"}
            socket.send(json.dumps(resume_command).encode('utf-8'))
            
        if cmd == "skip":
            socket.send(json.dumps({"command":"SKIP"}).encode('utf-8'))
            
    
   
    
    
    
    


if __name__ == "__main__":
    main()



    # # keep generating the playlist until each track has been proceesed and validated 
    # validated_tracks = Playlist(playlist_name="VALIDATED TRACKS FULL LIST BATCHING NOT SUPPORTED")
    
    # while(validated_tracks.get_size() < MASTER_PLAYLIST_OBJECT.get_size()):
    #     print(50*"=")
    #     llm_recomended_playlist_raw = LLMPlaylistGenerator.generate(MASTER_PLAYLIST_OBJECT.get_unplayed_tracks_name())
        
    #     for line in llm_recomended_playlist_raw:
    #         print(line)
    #     print(50*"=")
    #     # Check each raw output line for existence
    #     for track in llm_recomended_playlist_raw:
    #         if MASTER_PLAYLIST_OBJECT.exists_by_name(track):
    #             validated_tracks.add_track(MASTER_PLAYLIST_OBJECT.get_track_path_by_name(track))
    #             MASTER_PLAYLIST_OBJECT.set_track_played(track)
    #     print(f"Validated tracks {validated_tracks.get_size()} / {MASTER_PLAYLIST_OBJECT.get_size()}")
    
    # print(validated_tracks)
    
    
    
    # llm_recomended_playlist_raw = LLMPlaylistGenerator.generate(MASTER_PLAYLIST_OBJECT.get_unplayed_tracks_name())
    # print(llm_recomended_playlist_raw)
    
    # # Validate that every element of the raw list is a real track
    # validated_tracks_by_name = []
    # for output in llm_recomended_playlist_raw:
    #     if MASTER_PLAYLIST_OBJECT.exists_by_name(output):
    #         validated_tracks_by_name.append(output)
    # print(f"{len(validated_tracks_by_name)} tracks validated out of {MASTER_PLAYLIST_OBJECT.get_size()}")
    
    # # Set validated tracks to played
    # for track in validated_tracks_by_name:
    #     MASTER_PLAYLIST_OBJECT.set_track_played(track)
        
    # print("--- Loaded Master Playlist ---")
    # print(MASTER_PLAYLIST_OBJECT)
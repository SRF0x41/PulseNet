from beat_match import BeatMatcher
from socket_server import SocketServer
import json
import os
from queue import Queue

sample_tracks = [
    "Only Love Can Break Your Heart (Masters at Work Mix).mp3",
    "Disclosure - Latch ft. Sam Smith.mp3",
    "Roosevelt - Moving On (karlmixclub extended) version 1.mp3",
    "Skrillex - Slats Slats Slats [HQ].mp3",
    "Flume - Holdin On.mp3",
    "SNOW STRIPPERS - UNDER YOUR SPELL.mp3",
    "Flume - Sleepless feat. Jezzabell Doran.mp3",
    "This Summer (JBAG Remix).mp3",
    "MDK.mp3",
]


sample_track_list = [
    "Flume - Holdin On.mp3",
    "This Summer (JBAG Remix).mp3",
    "Skrillex - Slats Slats Slats [HQ].mp3",
    "MDK.mp3",
]


base_path = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/"
""" addTrack = {
        "command" " "ADD_NEXT",
        "track_path": (full_path)
        "properties": {
            "advance_start":0.0,
            "set_fade_in_duration":0.0,
            "set_virtual_end_trim":0.0,
            "set_fade_out_duration":0.0,
            "set_overlap_duration":0.0
        }
    }"""


def addTrackJson(
    path,
    advance_start=0.0,
    fadein_duration=0.0,
    virtual_end_trim=0.0,
    fadeout_duration=0.0,
    overlap_duration=0.0,
):
    """
    Build a JSON string for adding a track with properties.

    :param path: Path to the track
    :param advance_start: float
    :param fadein_duration: float
    :param virtual_end_trim: float
    :param fadeout_duration: float
    :param overlap_duration: float
    :return: JSON string
    """
    full_path = os.path.abspath(path)  # convert to full path

    addTrack = {
        "command": "ADD_NEXT",
        "track_path": full_path,
        "properties": {
            "advance_start": advance_start,
            "set_fade_in_duration": fadein_duration,
            "set_virtual_end_trim": virtual_end_trim,
            "set_fade_out_duration": fadeout_duration,
            "set_overlap_duration": overlap_duration,
        },
    }

    # Return as JSON string
    return json.dumps(addTrack)


def main():
    socket_server = SocketServer()
    print(sample_track_list)

    # Wait for client to connect;
    # socket_server.start()

    # Song queue
    # Create a queue
    beat_matcher = BeatMatcher()
    beat_match_queue = Queue(maxsize=2)  # optional max size
    
    current_track = base_path+sample_track_list.pop(0)
    beat_match_queue.put(current_track)
    
    next_track = base_path+sample_track_list.pop(0)
    beat_match_queue.put(next_track)
    
    while sample_track_list:
        q_list = list(beat_match_queue.queue)
        print(f"Current queue {q_list}")
        
        seconds_start_next = beat_matcher.match_last_n_bars(q_list[0],q_list[1])
        print("=" * 50)
        print(f"Start {q_list[1]} {seconds_start_next} before {q_list[0]} ends")
        
        # pop queue
        beat_match_queue.get()
        add_next = sample_track_list.pop(0)
        beat_match_queue.put(base_path+add_next)
    
    # prosess last two
    q_list = list(beat_match_queue.queue)
    seconds_start_next = beat_matcher.match_last_n_bars(q_list[0],q_list[1])
    print("=" * 50)
    print(f"Start {q_list[1]} {seconds_start_next} before {q_list[0]} ends")

        

    # beat_match_queue.put(base_path + sample_track_list.pop(0))
    # beat_match_queue.put(base_path + sample_track_list.pop(0))

    # beat_matcher = BeatMatcher()
    
    

    # for track in sample_track_list:
    #     # Peek last two tracks
    #     current_track = list(beat_match_queue.queue)[-2]  # last element
    #     next_track = list(beat_match_queue.queue)[-1]     # second last

    #     print("=" * 50)
    #     print(f"[QUEUE STATUS] Current Track : {current_track}")
    #     print(f"[QUEUE STATUS] Next Track    : {next_track}")

    #     # Compute next start time
    #     get_next_start_time = beat_matcher.match_last_n_bars(current_track, next_track)
    #     print(f"[BEAT MATCH] Start '{next_track}' {get_next_start_time:.3f} seconds before '{current_track}' ends")

    #     # Prepare next track
    #     full_path_next = base_path + track
    #     print(f"[QUEUE ACTION] Adding next track: {full_path_next}")

    #     # Maintain queue size
    #     beat_match_queue.get()  # removes oldest
    #     beat_match_queue.put(full_path_next)

    #     print(f"[QUEUE AFTER ADD] {list(beat_match_queue.queue)}")
    #     print("=" * 50, "\n")
        
    


    print("End")


if __name__ == "__main__":
    main()

"""

import json

# --- Create a JSON object ---
message = {
    "type": "beat_info",
    "bpm": 128.5,
    "bars": 8,
    "confidence": 0.92
}

# --- Serialize to a JSON string ---
json_str = json.dumps(message)
print("Serialized JSON:", json_str)

# --- Deserialize back to a Python dictionary ---
parsed_message = json.loads(json_str)
print("Parsed message:", parsed_message)

# --- Access fields ---
print("Type:", parsed_message["type"])
print("BPM:", parsed_message["bpm"])
print("Bars:", parsed_message["bars"])
print("Confidence:", parsed_message["confidence"])

    Valid commands
    
    addNext(track path)
    
    advanceStart(seconds relative to start)
    setFadeInDuration(seconds relative to start)
    setFadeOutDuration(seconds relative to end)
    setVirtualEndTrim(seconds relative to end)
    setOverlapDuration(Seconds relative to end of last song)
    
    addTrack = {
        "command" " "ADD_NEXT",
        "track_path": (full_path)
        "properties": {
            "advance_start":0.0,
            "set_fade_in_duration":0.0,
            "set_virtual_end_trim":0.0,
            "set_fade_out_duration":0.0,
            "set_overlap_duration":0.0
        }
    }
    


if __name__ == "__main__":
    
    song1 = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Flume - Holdin On.mp3"
    song2 = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/MDK.mp3"

    # Align to last 8 bars
    seconds_before_end = beat_match_last_n_bars(song1, song2, bars=8, beats_per_bar=4)

    print(f"Start song 2 {seconds_before_end:.3f} seconds before song 1 ends")
    
    addTrack = {
        "command" : "ADD_NEXT",
        "track_path": song1,
        "properties": {
            "advance_start":10.0,
            "set_fade_in_duration":7.0,
            "set_virtual_end_trim":15.0,
            "set_fade_out_duration":7.0,
            "set_overlap_duration":0.0
        }
    }
    
    # --- Serialize to a JSON string ---
            son_str = json.dumps(addTrack)
    print("Serialized JSON:", json_str)


    server = SocketServer(port=8080)
    server.start()
    
    

    #while True:
        
    server.send(json_str.encode("utf-8"))
    data = server.receive(1024)
    if not data:
        print("[SocketServer] Client disconnected")
        #break
    print("C++ says:", data.decode(errors="ignore"))

    server.shutdown()


    
   
    
    
    
    
    


if __name__ == "__main__":
    main()
"""

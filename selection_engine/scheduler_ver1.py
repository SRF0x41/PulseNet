from beat_match import BeatMatcher
from socket_server import SocketServer


sample_tracks = [
    "Only Love Can Break Your Heart (Masters at Work Mix).mp3",
    "Disclosure - Latch ft. Sam Smith.mp3",
    "Roosevelt - Moving On (karlmixclub extended) version 1.mp3",
    "Skrillex - Slats Slats Slats [HQ].mp3",
    "Flume - Holdin On.mp3",
    "SNOW STRIPPERS - UNDER YOUR SPELL.mp3",
    "Flume - Sleepless feat. Jezzabell Doran.mp3",
    "This Summer (JBAG Remix).mp3",
    "MDK.mp3"
]


sample_track_list = [
    "Flume - Holdin On.mp3",
    "This Summer (JBAG Remix).mp3",
    "Skrillex - Slats Slats Slats [HQ].mp3",
    "MDK.mp3"
    
]


base_path = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/"

def main():
    socket_server = SocketServer()
    socket_server.start()
    
    
    
    
    
if __name__ == "__main__":
    main()
    
'''

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
'''
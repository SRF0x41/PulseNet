from beat_match import BeatMatcher
from socket_server import SocketServer
import json
import os
from queue import Queue
from batch_queue_ver3 import BatchQueue
from audio_track import AudioTrack

sample_tracks = [
    "Disclosure - Latch ft. Sam Smith.mp3",
    "Roosevelt - Moving On (karlmixclub extended) version 1.mp3",
    "Skrillex - Slats Slats Slats [HQ].mp3",
    "Flume - Holdin On.mp3",
    "SNOW STRIPPERS - UNDER YOUR SPELL.mp3",
    "Flume - Sleepless feat. Jezzabell Doran.mp3",
    "This Summer (JBAG Remix).mp3",
    "MDK.mp3",
    "DJ Mehdi - Signatune (Thomas Bangalter Short Edit) [Official Audio].mp3",
]


sample_track_list = [
    "SNOW STRIPPERS - UNDER YOUR SPELL.mp3",
    "NIAMOS! (Chandrilian Club Mix) (From Andor (Season 2).mp3",
    "Skrillex - Slats Slats Slats [HQ].mp3",
    "Walking On A Dream by Empire Of The Sun (HQ Music).mp3",
    "DJ Mehdi - Signatune (Thomas Bangalter Short Edit) [Official Audio].mp3",
    "MDK.mp3",
    "SNOW STRIPPERS - UNDER YOUR SPELL.mp3",
    "Disclosure - Latch ft. Sam Smith.mp3",
    "Abracadabra (Gesaffelstein Remix).mp3",
    "Kinda Like It That Way - Snow Strippers.mp3",
    "Roosevelt - Moving On (karlmixclub extended) version 1.mp3",   
    
    
    "This Summer (JBAG Remix).mp3",
    "Flume - Holdin On.mp3",
    "MDK.mp3",
    "Flume - Sleepless feat. Jezzabell Doran.mp3",
]


BASE_PATH = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/"

# Prefix paths
sample_tracks = [BASE_PATH + track for track in sample_tracks]
sample_track_list = [BASE_PATH + track for track in sample_track_list]
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
        "properties": {
            "track_path": full_path,
            "advance_start": advance_start,
            "fade_in_duration": fadein_duration,
            "virtual_end_trim": virtual_end_trim,
            "fade_out_duration": fadeout_duration,
            "overlap_duration": overlap_duration,
        },
    }

    # Return as JSON string
    return json.dumps(addTrack)


FIXED_ADVANCE_START = 10
FIXED_FADE_IN_DURATION = 5
FIXED_FADE_OUT_DURATION = 5





    

def main():
    print("Starting PulseNet beat-match preview...\n")
    
    # Await client connection 
    socket = SocketServer()
    socket.start()

    # Show remaining tracks before any processing
    batch_queue = BatchQueue(sample_track_list)

    next_track = batch_queue.getNext()
    while next_track:
        print(next_track)
        track_json = json.dumps(next_track.to_json())
        socket.send(track_json.encode('utf-8'))
        next_track = batch_queue.getNext()


if __name__ == "__main__":
    main()


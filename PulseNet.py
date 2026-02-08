import readline
from pathlib import Path

TRACK_PATH = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/"

VALID_COMMANDS = {
    "help",
    "exit",
    "quit",
    "get-tracks",
    "create-playlist",
    "get-all-playlists"
}

TEMPORARY_PLAYLISTS = []


def print_dir(path):
    for item in Path(path).iterdir():
        print(item.name)
        
def print_temporary_playlists():
    for playlist in TEMPORARY_PLAYLISTS:
        print(playlist[0])
        


def main():
    print("--- PulseNet ---")
    print("Interactive mode. Type 'exit' or 'quit' to leave.")

    while True:
        try:
            raw = input("> ").strip()
        except EOFError:
            print()
            break

        if not raw:
            continue

        parts = raw.split()
        command = parts[0]
        args = parts[1:]

        if command in ("exit", "quit"):
            break
        
  
            

        if command == "get-all":
            print_dir(path)
        if command == "create-playlist":
            if len(args) > 1:
                print(f"Invalid number of arguments, expected 1 but got {len(args)}")
                break
            if len(args) == 0:
                print("No playlist name provided")
                
            new_playlist = [args]
            TEMPORARY_PLAYLISTS.append(new_playlist)
            
        if command == "get-all-playlists":
            print_temporary_playlists()
        else:
            print(f"Unknown command: {command}")



if __name__ == "__main__":
    main()

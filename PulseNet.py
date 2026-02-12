import readline
from pathlib import Path

TRACK_PATH = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/"

VALID_COMMANDS = {
    "help",
    "exit",
    "quit",
    "get-tracks",
    "create-playlist",
    "get-all-playlists",
    "use-playlist"
}

COMMAND_DESCRIPTIONS = {
    "help": "Show this help message",
    "exit": "Exit PulseNet",
    "quit": "Exit PulseNet",
    "get-tracks": "List all available tracks",
    "create-playlist": "Create a new temporary playlist (usage: create-playlist <name>)",
    "get-all-playlists": "List all temporary playlists",
    "use-playlist": "Select a playlist to use"
}

TEMPORARY_PLAYLISTS = []


def print_dir(path):
    for item in Path(path).iterdir():
        print(item.name)


def print_temporary_playlists():
    if not TEMPORARY_PLAYLISTS:
        print("No playlists created.")
        return

    for playlist in TEMPORARY_PLAYLISTS:
        print(playlist)


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

        if command not in VALID_COMMANDS:
            print(f"Unknown command: {command}")
            continue

        # --------------------
        # HELP
        # --------------------
        if command == "help":
            print("Available commands:")
            for cmd in VALID_COMMANDS:
                description = COMMAND_DESCRIPTIONS.get(
                    cmd, "No description available"
                )
                print(f"  {cmd:<20} - {description}")
            continue

        # --------------------
        # EXIT
        # --------------------
        if command in ("exit", "quit"):
            break

        # --------------------
        # GET TRACKS
        # --------------------
        if command == "get-tracks":
            print_dir(TRACK_PATH)
            continue

        # --------------------
        # CREATE PLAYLIST
        # --------------------
        if command == "create-playlist":
            if len(args) != 1:
                print(
                    f"Invalid number of arguments. Usage: create-playlist <name>"
                )
                continue

            playlist_name = args[0]

            if playlist_name in TEMPORARY_PLAYLISTS:
                print("Playlist already exists.")
                continue

            TEMPORARY_PLAYLISTS.append(playlist_name)
            print(f"Playlist '{playlist_name}' created.")
            continue

        # --------------------
        # GET ALL PLAYLISTS
        # --------------------
        if command == "get-all-playlists":
            print_temporary_playlists()
            continue


if __name__ == "__main__":
    main()

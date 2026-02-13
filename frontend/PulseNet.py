import readline
import shlex
from pathlib import Path
from playlist import Playlist  # your existing Playlist class

TRACK_PATH = "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/"

VALID_COMMANDS = {
    "help",
    "exit",
    "quit",
    "get-tracks",
    "create-playlist",
    "get-all-playlists",
    "use-playlist",
    "current-playlist"
}

COMMAND_DESCRIPTIONS = {
    "help": "Show this help message",
    "exit": "Exit PulseNet",
    "quit": "Exit PulseNet",
    "get-tracks": "List all available tracks in the master playlist",
    "create-playlist": "Create a new temporary playlist (usage: create-playlist <name>)",
    "get-all-playlists": "List all temporary playlists",
    "use-playlist": "Select a playlist to use (usage: use-playlist <name>)",
    "current-playlist": (
        "Show or manage the current playlist.\n"
        "Usage:\n"
        "  current-playlist              Show current playlist name\n"
        "  current-playlist -a <track1> <track2> ...  Add tracks from master playlist"
    )
}

# Master tracks (all local mp3s)
MASTER_TRACKS = {}
PLAYLISTS = {}           # {name: Playlist object}
CURRENT_PLAYLIST = None  # Playlist object


def get_tracks():
    """Return dictionary of all mp3 files in TRACK_PATH, sorted alphabetically."""
    path = Path(TRACK_PATH)
    if not path.exists():
        return {}

    tracks = {
        item.name: str(item.resolve())
        for item in path.iterdir()
        if item.is_file() and item.suffix.lower() == ".mp3"
    }
    return dict(sorted(tracks.items(), key=lambda x: x[0].lower()))


def print_tracks(tracks_dict):
    """Print keys of a dict vertically."""
    for i, key in enumerate(tracks_dict, start=1):
        print(f"{i}. {key}")


def completer(text, state):
    buffer = readline.get_line_buffer()
    stripped = buffer.lstrip()
    try:
        parts = shlex.split(stripped)
    except ValueError:
        parts = stripped.split()

    # Determine what the current "word" is
    if buffer.endswith(" "):
        parts.append("")  # allow completion for new word

    options = []

    # No input yet → suggest commands
    if len(parts) == 0:
        options = [cmd for cmd in VALID_COMMANDS if cmd.startswith(text)]
    else:
        command = parts[0]

        # Complete first word (command)
        if len(parts) == 1:
            options = [cmd for cmd in VALID_COMMANDS if cmd.startswith(text)]

        # Playlist name completion
        elif command == "use-playlist":
            options = [p for p in PLAYLISTS if p.startswith(text)]

        # Track name completion
        elif command == "current-playlist" and "-a" in parts:
            options = [t for t in MASTER_TRACKS if t.startswith(text)]

    try:
        return options[state]
    except IndexError:
        return None


def main():
    global MASTER_TRACKS, CURRENT_PLAYLIST

    print("--- PulseNet ---")
    print("Interactive mode. Type 'exit' or 'quit' to leave.")

    MASTER_TRACKS = get_tracks()
    master_playlist = Playlist(master_playlist=MASTER_TRACKS)

    # Setup tab completion
    readline.set_completer(completer)
    readline.parse_and_bind("tab: complete")

    while True:
        try:
            raw = input("> ").strip()
        except EOFError:
            print()
            break

        if not raw:
            continue

        # Use shlex to handle quotes
        try:
            parts = shlex.split(raw)
        except ValueError as e:
            print(f"Invalid input: {e}")
            continue

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
            for cmd in sorted(VALID_COMMANDS):
                desc = COMMAND_DESCRIPTIONS.get(cmd, "")
                print(f"  {cmd:<20} - {desc}")
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
            tracks = master_playlist.get_tracks_name()
            print_tracks(tracks)
            continue

        # --------------------
        # CREATE PLAYLIST
        # --------------------
        if command == "create-playlist":
            if len(args) != 1:
                print("Usage: create-playlist <name>")
                continue
            playlist_name = args[0]
            if playlist_name in PLAYLISTS:
                print("Playlist already exists.")
                continue
            PLAYLISTS[playlist_name] = Playlist(playlist_name=playlist_name)
            print(f"Playlist '{playlist_name}' created.")
            continue

        # --------------------
        # GET ALL PLAYLISTS
        # --------------------
        if command == "get-all-playlists":
            if not PLAYLISTS:
                print("No playlists created.")
            else:
                for key in PLAYLISTS:
                    print(key)
            continue

        # --------------------
        # USE PLAYLIST
        # --------------------
        if command == "use-playlist":
            if len(args) != 1:
                print("Usage: use-playlist <name>")
                continue
            playlist_name = args[0]
            if playlist_name not in PLAYLISTS:
                print(f"Playlist '{playlist_name}' does not exist.")
                continue
            CURRENT_PLAYLIST = PLAYLISTS[playlist_name]
            print(f"Now using playlist '{playlist_name}'.")
            continue

        # --------------------
        # CURRENT PLAYLIST
        # --------------------
        if command == "current-playlist":
            if CURRENT_PLAYLIST is None:
                print("No playlist selected. Use 'use-playlist <name>'.")
                continue

            if not args:
                # Print playlist name
                print(f"Current playlist: {CURRENT_PLAYLIST.get_name()}")
                continue

            # Add tracks: current-playlist -a <track1> <track2> ...
            if args[0] == "-a" and len(args) > 1:
                for track_name in args[1:]:
                    if track_name not in MASTER_TRACKS:
                        print(f"Track '{track_name}' does not exist.")
                    else:
                        track_path = MASTER_TRACKS[track_name]
                        CURRENT_PLAYLIST.add_track(track_path)
                        print(f"Added '{track_name}' to playlist.")
                continue


if __name__ == "__main__":
    main()

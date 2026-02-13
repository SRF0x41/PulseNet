from pathlib import Path


class Playlist:
    # Structure:
    # {
    #   "track_name": [full_path, played_boolean]
    # }

    def __init__(self, playlist_track_paths=None, playlist_name=None):
        self.playlist_name = playlist_name
        self.playlist = {}

        if playlist_track_paths:
            self.playlist = self._build_playlist(playlist_track_paths)

    # -------------------------
    # Internal Builder
    # -------------------------
    def _build_playlist(self, playlist_track_paths):
        mod_playlist = {}

        for track in playlist_track_paths:
            name = Path(track).stem
            mod_playlist[name] = [track, False]

        return mod_playlist

    # -------------------------
    # Playlist Metadata
    # -------------------------
    def set_playlist_name(self, playlist_name):
        self.playlist_name = playlist_name

    def get_name(self):
        return self.playlist_name

    # -------------------------
    # Track Management
    # -------------------------
    def add_track(self, track_path):
        name = Path(track_path).stem

        if name not in self.playlist:
            self.playlist[name] = [track_path, False]

    def set_playlist(self, playlist_track_paths):
        self.playlist = self._build_playlist(playlist_track_paths)

    def set_track_played(self, track_name):
        if track_name in self.playlist:
            self.playlist[track_name][1] = True

    def reset_track_played(self, track_name):
        if track_name in self.playlist:
            self.playlist[track_name][1] = False

    # -------------------------
    # Getters
    # -------------------------
    def get_all_tracks_name(self):
        return list(self.playlist.keys())

    def get_all_tracks_path(self):
        return [data[0] for data in self.playlist.values()]
    
    def get_track_path_by_name(self,track_name):
        return self.playlist[track_name][0]
        
    def get_size(self):
        return len(self.playlist.keys())

    def get_playlist(self):
        return self.playlist

    def get_track_info(self, track_name):
        return self.playlist.get(track_name)
    
    def get_unplayed_tracks_name(self):
        unplayed = []
        for track in self.playlist.keys():
            if not self.playlist[track][1]:
                unplayed.append(track)
        return unplayed
    
    def get_unplayed_tracks_path(self):
        unplayed = []
        for track in self.playlist.keys():
            if not self.playlist[track][1]:
                unplayed.append(self.playlist[track][0])
        return unplayed
        

    # -------------------------
    # Existence Checks
    # -------------------------
    def exists_by_name(self, track_name):
        return track_name in self.playlist

    def exists_by_path(self, track_path):
        for path, _ in self.playlist.values():
            if path == track_path:
                return True
        return False
    
    # -------------------------
    # String Representation
    # -------------------------
    def __str__(self):
        lines = []
        lines.append(f"Playlist: {self.playlist_name or 'Unnamed'}")
        lines.append("-" * 40)
        for name, (path, played) in self.playlist.items():
            status = "Played" if played else "Not Played"
            lines.append(f"{name} [{status}]")
        return "\n".join(lines)
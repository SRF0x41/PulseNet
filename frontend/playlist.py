class Playlist:
    # playlist: { track_name : full_path }

    def __init__(self, master_playlist=None, playlist_name=None):
        self.playlist_name = playlist_name
        self.playlist = master_playlist if master_playlist else {}

    def set_name(self, playlist_name):
        self.playlist_name = playlist_name

    def add_track(self, track_path):
        """
        Adds a track to the playlist.
        If the path already exists in the playlist, do nothing.
        """

        # If path already exists in values → do nothing
        if track_path in self.playlist.values():
            return

        # Extract file name from path
        from pathlib import Path
        track_name = Path(track_path).name

        # Add to dictionary
        self.playlist[track_name] = track_path

    def set_master_playlist(self, all_tracks):
        self.playlist = all_tracks
        
        
    def get_name(self):
        return self.playlist_name
        
        
    def get_tracks_name(self):
        return self.playlist.keys()
    
    def get_tracks_path(self):
        return self.playlist.values()
    
    def get_playlist(self):
        return playlist
    
    def exists(self,track_name):
        if track_name in self.playlist.keys():
            return True
        return False
    
    def exist(self,track_path):
        if track_path in self.playlist.values():
            return True
        return False
        

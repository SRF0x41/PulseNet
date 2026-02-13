from ollama import chat
from typing import Final, List
import os


class LLMPlaylistGenerator:
    MODEL: Final = "llama2:13b"

    SYSTEM_PROMPT: Final = """
        You are a house party DJ playlist generator.

        You are an elite house party DJ AI. Your job is to build high-energy, 
        crowd-pleasing playlists that flow smoothly.

        CRITICAL OUTPUT RULES:
        - Output ONLY the track names.
        - One track per line.
        - No numbering.
        - No bullets.
        - No commentary.
        - No explanations.
        - No intro text.
        - No outro text.
        - No blank lines.
        - No additional words.
        - Do not wrap in quotes.
        - Do not format in markdown.
        """

    # ------------------------
    # Public API
    # ------------------------
    @staticmethod
    def generate(track_list: List[str]) -> List[str]:
        cleaned_tracks = LLMPlaylistGenerator._clean_tracks(track_list)
        prompt = LLMPlaylistGenerator._build_prompt(cleaned_tracks)

        response = chat(
            LLMPlaylistGenerator.MODEL,
            messages=[
                {"role": "system", "content": LLMPlaylistGenerator.SYSTEM_PROMPT},
                {"role": "user", "content": prompt},
            ],
        )

        output = response.message.content.strip()
        playlist = [line.strip() for line in output.split("\n") if line.strip()]

        LLMPlaylistGenerator._validate_output(playlist, cleaned_tracks)

        return playlist

    # ------------------------
    # Internal Helpers
    # ------------------------
    @staticmethod
    def _clean_tracks(track_list: List[str]) -> List[str]:
        seen = set()
        cleaned = []

        for track in track_list:
            name = os.path.splitext(track)[0].strip()
            if name not in seen:
                seen.add(name)
                cleaned.append(name)

        return cleaned

    @staticmethod
    def _build_prompt(cleaned_tracks: List[str]) -> str:
        track_block = "\n".join(cleaned_tracks)

        return f"""
            Reorder the following tracks into a DJ-style house party flow.

            MANDATORY RULES:
            - Use every track exactly once.
            - Do not skip tracks.
            - Do not duplicate tracks.
            - Do not modify track names.
            - Do not invent tracks.
            - Final output must contain exactly {len(cleaned_tracks)} lines.
            - Each line must be exactly one of the provided track names.
            - Output must be plain raw text only.

            Tracks:
            {track_block}
            """

    @staticmethod
    def _validate_output(playlist: List[str], cleaned_tracks: List[str]) -> None:
        if len(playlist) != len(cleaned_tracks):
            raise ValueError("Model output length mismatch.")

        if set(playlist) != set(cleaned_tracks):
            raise ValueError("Model output contains invalid or missing tracks.")

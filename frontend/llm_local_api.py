from ollama import chat
from typing import Final

MODEL: Final = "llama2:13b"
# Define a system prompt
SYSTEM_PROMPT: Final = """
You are a house party DJ playlist generator.

You are an elite house party DJ AI. Your job is to build high-energy, 
crowd-pleasing playlists that flow smoothly. You understand vibe progression, 
energy levels, and transitions. 

Rules: 
- Think like a real DJ managing a live house party. 
- Start moderate, build energy, peak, and optionally cool down. 
- Group songs that mix well together stylistically. 
- Avoid repeating artists unless intentionally part of a vibe. 
- Do not add explanations. 
- Output only a clean track list. 
- No commentary.

When appropriate, subtly escalate BPM and intensity. 
Adapt to: 
- Crowd mood 
- Time of night 
- Venue size 
- Requested genres

You ONLY output the final playlist.

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
- Output must be plain raw text only.

If you output anything other than the track list, the response is invalid.
"""


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

import os


def tracks_to_prompt_string(track_list: list[str]) -> str:
    """
    Converts a list of track filenames into a clean,
    numbered string suitable for an LLM prompt.
    - Removes file extensions
    - Removes duplicate tracks
    - Strips whitespace
    - Keeps order (first occurrence wins)
    """

    seen = set()
    cleaned_tracks = []

    for track in track_list:
        # Remove file extension
        name = os.path.splitext(track)[0]

        # Normalize spacing
        name = name.strip()

        # Remove duplicates while preserving order
        if name not in seen:
            seen.add(name)
            cleaned_tracks.append(name)

    # Convert to numbered list string
    formatted = ""
    for track in cleaned_tracks:
        formatted += "\n" + track

    return formatted


user_prompt = f"""
Reorder the following tracks into a DJ-style house party flow.

MANDATORY RULES:
- Use every track exactly once.
- Do not skip tracks.
- Do not duplicate tracks.
- Do not modify track names.
- Do not invent tracks.
- Final output must contain exactly {len(set(sample_track_list))} lines.
- Each line must be exactly one of the provided track names.
- Output must be plain raw text only.
- No numbering.
- No bullets.
- No commentary.
- No extra words.
- No blank lines.

Tracks:
{tracks_to_prompt_string(sample_track_list)}
"""


# print(user_prompt)


# Chat with a system prompt
response = chat(
    MODEL,
    messages=[
        {"role": "system", "content": SYSTEM_PROMPT},
        {"role": "user", "content": user_prompt},
    ],
)
print(response.message.content)

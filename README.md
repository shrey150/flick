# 🎬 Flick: “AI-driven video editing”

This is the most recent iteration in a long dream of mine since 5th grade to build a fully-fledged video editor.

In its past lives, it's been:

- A screen recorder called "Flick Studios" written in Java. Used an ancient AV library called [Xuggler](https://www.xuggle.com/xuggler) to stich together screenshots, which was a pain in the ass to deal with: it inexplicably inserted random frames in the middle of my recordings. Good memories.
- An Electron app as my first foray into desktop app development. Only got far enough to emulate an iMovie-esque clipbin and timeline (using a memorably fun library called [Dragula](https://github.com/bevacqua/dragula)), before my monstrous spaghetti codebase left me defeated.

Now, it's been reincarnated (and promptly scrapped) as an AI-powered video editing utility, meant to solve a personal pain point setting up Premiere Pro projects for simple video splicing (with the alternative being wrestling with the FFmpeg CLI).

I've paused development for now, but I'm open-sourcing my progress in case that motivates me to come back to it someday. I've included the most recent design doc in case it's useful (circa December 2021).

---

## 🧠 Core values

- Light editing without project files, imports, and long load times
- Open the app, do what you need, and leave immediately
- Simple, intuitive video editing enhanced by AI
- Try to avoid making a worse version of iMovie

## 🚀 Features

- Video to GIF, general file conversion
- Download YouTube videos (using [youtube-dl](https://github.com/ytdl-org/youtube-dl))
    - Suggest downloading if a video link is on their clipboard ([library](https://github.com/dacap/clip))
- Screen record? (Use native APIs for extra intuitive feel)
- AI-suggested trimming
    - Remove [black](https://ffmpeg.org/ffmpeg-filters.html#blackdetect), paused (YT), **Snapchat, iOS screen record**, Twitch, silent
        - **FULL LIST:** YT, Snapchat, iOS, Twitch, IG Live, TikTok, etc
        - These sections could be Gaussian blurred while video is obstructed
    - Drop videos in and AI cuts/splices them into a single video (eg. [Tailor](https://apps.apple.com/us/app/tailor-screenshot-stitching/id926653095))
        - Edit speed of video really easily, trim length of it using timeline, etc
    - Compress video to common size formats (8MB [Discord])
        - Predict file size of diff compression outputs and choose min amount
    - Rotate video, add black bars or blurred bars for portrait video
    - **Crop video** to different aspect ratios using scalable rect, just like photos
    - NECESSARY: Preserve FPS in all edits, lossless export, etc
- MAJOR Q: audio/music support? Don’t want too much editing though like iMovie
- Nice to haves (but not necessary): filters, montages (both gimmicky imo)

## 🎨 UI/Usage

- Small “record button row” in the right corner for screen capture/video/mic record
- Big “drag-and-drop” area for user to drop in files, also an open file dialog
- Once files are dropped in, timeline appears with suggestions on different parts
    - Like “squiggly lines” for text, different colors and icons show suggestions
    - (UI and idea for app inspired by this video from Google)
    - Hovering over provides suggestions, and users can trim/blur/add effects
- DON’T try to add too many editing features (eg. probably won’t have “split clip”)
- (If green screen is supported) user can drop green screen video on top
    - If clips aren’t same length, user would select which parts to superimpose

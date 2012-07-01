pulsemixer
==========

Pulsemixer is a console volume control application for the pulseaudio server.
You can adjust the volume of audio interfaces and running applications.
Compared to [pavucontrol](http://freedesktop.org/software/pulseaudio/pavucontrol/) it is very limited.
For instance you can only see/change the average volume of left and right channels.
Also there is no built-in maximum volume so use it with caution!

## Usage

This is how it looks on my machine:

    [   0.00 dB] High Definition Audio Controller Digital Stereo (HDMI)   
    [  -0.81 dB] Built-in Audio Analog Stereo
    [  -5.27 dB] Quod Libet : 'I'll Never Know' by 'KID'
    [ -15.23 dB] VLC media player : audio stream
    [  -0.81 dB] ALSA plug-in [chromium] : ALSA Playback

Change selection with `j`,`k` keys. To change the volume use `h` and `l`, hold shift for fine tuning.
Press `m` to set the volume to -inf dB (0%) and `n` to set it to 0.0 dB (100%).

## Building

To compile you need libncurses and libpulse. Then just run

    $ cd src/
    $ make

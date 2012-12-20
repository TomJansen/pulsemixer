pulsemixer
==========

Pulsemixer is a console volume control application for the pulseaudio server.
You can adjust the volume of audio interfaces and running applications.
Compared to [pavucontrol](http://freedesktop.org/software/pulseaudio/pavucontrol/) it is very limited.
For instance you can only see/change the average volume of left and right channels.
Also there is no built-in maximum volume so use it with caution!

## Usage

Use keybindings in terminal

This is how it looks on my machine:

     arrow keys - navigate and adjust volume
              n - set volume to 0.00 dB
              m - mute
              q - quit

    [   0.00 dB] High Definition Audio Controller Digital Stereo (HDMI)            
    [  -9.29 dB] Built-in Audio Analog Stereo
    [  -1.22 dB] VLC media player : audio stream
    [   0.00 dB] Spotify : Spotify


## Install

To compile you need libncurses and libpulse. Then just run

    $ cd src/
    $ make
    $ sudo make install

## Recommended

To get independent volume controls of sink output and attached clients set `flat-volumes = no` in `/etc/pulse/daemon.vonf`.
This is the default setting in Ubuntu 12.10.

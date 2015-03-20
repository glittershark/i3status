i3status - glittershark's fork
==============================

i3status is a small program for generating a status bar for i3bar, dzen2,
xmobar, or similar programs. It is designed to be very efficient by issuing a
very small number of system calls, as one generally wants to update such a
status line every second. This ensures that even under high load, your status
bar is updated correctly. Also, it saves a bit of energy by not hogging your
CPU as much as spawning the corresponding amount of shell commands would.

### This fork adds:

- Configurable display of the currently playing MPD track
- Notifications of MPD track changes using libnotify
- Notifications of battery state change and critical level with libnotify

## Installation

Currently there is no official package for this fork of i3status, so you'll have
to compile it yourself. Fortunately that's pretty easy - just clone the
repository and run the following:

```
make && sudo make install
```

## Dependencies

i3status has the following dependencies:
- libconfuse-dev
- libyajl-dev
- libasound2-dev
- libiw-dev
- libnotify
- libmpdclient
- libcap2-bin (for getting network status without root permissions)
- asciidoc (only for the documentation)


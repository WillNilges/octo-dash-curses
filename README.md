# Octo Dash Curses

A simple, rugged dashboard for your print server.

octo-dash-curses (odc) is an app written in C that provides a dead-simple way to check on your 3D prints via an OctoPrint server through your terminal. You can install it on your OctoPrint server and hook up a display to monitor your print progress without the need for a display server, or install it on your computer for a quicker way to check your print progress.

## Installation

### Dependencies

#### Fedora

- gcc
- libcurl-devel
- ncurses-devel
- libconfig-devel

### Building

- Clone the repo, run `make`, and put `odc` somewhere on your `$PATH` along with an `api.cfg` file.
- Run it, and you'll be presented with a dashboard that looks something like this:

```
  Yo, what's cookin!?

    Print name: frontal_0.12mm_PLA_ENDER3_5h6m.gcode
         Owner: wilnil
  Time elapsed: 3 hr, 47 min, 21 sec
      Progress: 72%
  [#####################################              ]

```


## Configuration

You can configure stuff through `api.cfg`

- `url`: The URL to your OctoPrint server
- `key`: The API key to allow `odc` to get access to your server
- `refresh`: The refresh rate of `odc`'s data, in seconds. (On _my_ LAN, I can put this down to 1 second and it works fine, but you might want to slow it down if your print server is slow or your connection is slow.
- 'border': The distance from the left OCD prints stuff.
- 'scale': The length of the progress bar.

## Development

First of all, thanks!

Compilation is dead simple, use whatever for your IDE, and don't commit stupid code.

Open issues, open PRs, fork to your heart's content. I intended this to just be a monitoring thing, but establishing this as a bonafide control panel might be kinda cool.

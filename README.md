# Octo Dash Curses

A simple, rugged dashboard for your print server.

octo-dash-curses (odc) is an app written in C that provides a dead-simple way to check on your 3D prints via an OctoPrint server through your terminal. You can install it on your OctoPrint server and hook up a display to monitor your print progress without the need for a display server, or install it on your computer for a quicker way to check your print progress.

### How it works

ODC queries your OctoPrint server for printer status and displays the most relevant information about your print. It runs in a loop until you hit `^C`.

![image](https://user-images.githubusercontent.com/42927786/91780262-026cf080-ebc5-11ea-962f-c456396d84f3.png)


### Supported Printers

- Ender 3 Pro
- _probably others but the Ender 3 Pro is the only one I have_

## Installation

### Dependencies

#### Fedora

- gcc
- libcurl-devel
- ncurses-devel
- libconfig-devel

#### Raspbian

- gcc
- libconfig-dev
- libncurses-dev
- libcurl4-gnutls-dev

### Building

- Clone the repo, run `make`, and put `odc` somewhere on your `$PATH` along with an `api.cfg` file.
- Run it, and you'll be presented with a dashboard that looks something like this:

```
  Yo, what's cookin!?                                                      
                                                                           
    Print name: Phone_stand_flippable_stand_0.12mm_PLA_ENDER3_3h19m.gcode  
         Owner: wilnil                                                     
                                                                           
    Print head: 27.27/0.0 °C                                               
           Bed: 27.27/0.0 °C                                               
                                                                           
  Time elapsed: 3 hr, 38 min, 30 sec                                       
      Progress: 100%                                                       
  [▒▒▒▒▒▒▒▒▒▒▒.▒▒▒▒▒▒▒▒▒▒▒▒.▒▒▒▒▒▒▒▒▒▒▒.▒▒▒▒▒▒▒▒▒▒▒▒]                      
  Done!                                                                                                                                 
```

## Configuration

You can configure stuff through `api.cfg`

- `url`: The URL to your OctoPrint server
- `key`: The API key to allow `odc` to get access to your server
- `refresh`: The refresh rate of `odc`'s data, in seconds. (On _my_ LAN, I can put this down to 1 second and it works fine, but you might want to slow it down if your print server is slow or your connection is slow)
- `border`: The distance from the left OCD prints stuff.
- `scale`: The length of the progress bar.

## Development

First of all, thanks!

Compilation is dead simple, use whatever for your IDE, and don't commit stupid code.

Open issues, open PRs, fork to your heart's content. I intended this to just be a monitoring thing, but establishing this as a bonafide control panel might be kinda cool.

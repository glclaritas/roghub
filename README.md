# ROG Hub
A small cli tool to control ROG laptop's power profile mode and fan mode.  
This is a do-as-you-learn hobby project.

#### Features
 - [x] 3 profile switching[Silent, Balanced, Performance]
 - [x] Separate fan mode control
 - [x] Wayland On-Screen-Display (need to have notify-send)
 - [ ] GUI
 - [x] Custom Profile

 #### Requirements
  - asus-nb-wmi kernel module for fan control.
  - provided udev rules for rootless control.

 #### Building

    mkdir build && cd build
    cmake .. && make

 #### Installation
    sudo make install

 #### Config file format  

    name=<profilename>  
    maxghz=4.5  
    fanmode=3  
    turbo=1  

fanmode values: { 1 | 2 | 3 }   { silent | balanced | turbo }
turbo values: { 1 | 0 }         { on | off }

See usage with --help

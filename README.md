\<logo maybe here?\>
## Xupric __[alpha]__
__[Project is in its early stages!]__

A Modern Web Browser based on webkit2gtk.\
Moderately inspired by surf.

### Features
* Functional web searching
* Tabs support (10 static tabs)
* Configuration through ~/.config/xupric/xupric.conf
* Additional (styling, certs) configuration in ~/.config/xupric/
* Shortcut keys
* Custom Navigation Bar made with gtkbuilder and css
* & much more

### Dependencies
* <a href="https://github.com/libconfuse/libconfuse">libconfuse</a> \<required\>
* webkit2gtk \<required\>
* meson \<required\>
* x11 \<required\>
* gtk3 \<required\>

### Install
```sh
> mkdir build && cd build
> meson ..
> ninja
> sudo ninja install 
```
NB! For a way better look, change duckduckgo's theme to __dark__.

### Showcase
<img src="res/previews/xupric_start.png">
<img src="res/previews/xupric_menu.png">
<img src="res/previews/xupric_wiki.png">

### Keyboard Shortcuts
| KEY            | Action           |
| ------         | -----            |
| alt+q          | quit             |
| ctrl+r         | reload           |
| ctrl+shift+r   | reload -cache    |
| F5             | reload           |
| Esc            | stop loading     |
| F11            | fullscreen toggle|
| alt+h          | goto home/start  |
| alt+Left       | go backwards     |
| alt+Right      | go forwards      |
| ctrl+equal     | zoom in          |
| ctrl+minus     | zoom out         |
| ctrl+0         | zoom reset       |
| ctrl+Tab       | next tab         |
| ctrl+shift_Tab | previous tab     |
| alt+\<x\>      | switch to tab x  |

Tab (10 max) switching is done with alt+(tab_number from 1->0)

### Configuration
Most of the configuration is done through ~/.config/xupric/xupric.conf.\
And the styles are in ~/.config/xupric/styles/

### Credits
Thank you, <a href="https://github.com/Tux-Code">Tux-Code</a> - OvercoME for creating the logos.

### Notes
The default xupric is styled using the <a href="https://github.com/ChrisKempson/Tomorrow-Theme">Tomorrow Night</a> color theme.\
The cache is stored in ~/.cache/xupric/\
Almost all the files/dirs get generated during run-time.\
The coding style is somewhat a lazy Linux kernel like.\
There are a ton of bugs, so hold your teddy bear.

## <img src="res/xupric.png" width=18> Xupric

Xupric is a fully functional Web browser.\
The features and look was inspired by surf, Firefox and DuckDuckGo's Privacy Browser.\
It's goal was to implement all the features you'd expect from a today's browser,\
while keeping a nice, dark, non-GTK look, to go along with any wm.\
It was written from scratch and isn't a fork.

### Features
* Functional web searching
* Tabs support (10 static tabs)
* Custom GTK ui
* Dark mode
* Workspaces (max 5)
* Url filtering like clearURLs
* a Ton of Shortcut keys
* Bookmarks, history, cookies
* Configuration through ~/.config/xupric/xupric.conf
* Custom css/js/certs in ~/.config/xupric/
* & much more

### Dependencies
* <a href="https://github.com/libconfuse/libconfuse">confuse</a> \<required\>
* webkit2gtk-4.0 \<required\>
* jansson \<required\>
* pcre \<required\>
* xxd \<required\>
* sqlite3 \<required\>
* meson \<required\>
* x11 \<required\>
* gtk3 \<required\>
* <a href="https://www.nerdfonts.com/font-downloads">JetBrainsMono Nerd Font</a> \<very recommended\>
* <a href="https://github.com/cbrnix/Flatery">Flatery-Dark icon theme</a> \<very recommended\>

__Without the nerd font, the browser looks very weird.__\
Gtk4 and webkit2gtk-5.0 __aren't__ supported.

### Install
```sh
> git clone https://github.com/chm46e/xupric
> cd xupric
> mkdir build && cd build
> meson ..
> ninja
> sudo ninja install
```
__NB! Have a look at the man page!__ ```man xupric```\
It goes over everything (shortcuts, features, etc).

### Showcase
<img src="res/previews/xupric_start.png">
<img src="res/previews/xupric_wiki.png">
<img src="res/previews/xupric_menu.png">

### Credits
Thank you, <a href="https://github.com/Tux-Code">Tux-Code</a> for creating the logos.

### Notes
The default xupric is styled using the <a href="https://github.com/ChrisKempson/Tomorrow-Theme">Tomorrow Night</a> color theme.\
The configuration stuff is stored in ~/.config/xupric/\
Almost all the files/dirs get generated during run-time.\
The coding style is somewhat a lazy Linux kernel like.\
Again, have a look at xupric's man page:D\
There are a ton of bugs, so hold your teddy bear.

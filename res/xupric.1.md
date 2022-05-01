% Xupric(1)

# NAME
xupric - a Modern Web Browser based on WebKit2GTK

# SYNOPSIS
**xupric** [-pdvh] [uri]

# DESCRIPTION
Xupric is a fully functional Web browser. The features and look was inspired by surf, Firefox and DuckDuckGo's Privacy Browser. It's goal was to implement all the features you'd expect from a today's browser, while keeping a nice, dark, non-GTK look, to go along with any wm.

# OPTIONS
**-p**, **--private**
: Enables ephemeral aka private mode.

**-d**, **--debug**
: Enables debugging

**-v**, **--version**
: Shows version info

**-h**, **--help**
: Displays a help message

# USAGE
**NAVIGATION**\
Right click in a empty space opens a navigation popup.\
There are no "normal" buttons for navigation, as\
shortcuts or popup are intended to be used.\
---------------------------------------------------\
**alt+Left** - go back in history\
**alt+Right** - go forward in history\
**Esc** - stop loading page\
**alt+h** - go to home\
**ctrl+r**, **F5** - reload page\
**ctrl+shift+r** - reload page with bypass cache\
**alt+w**, **midclick uri** - open a new window\
other shortcuts are defined by webkit2gtk..\
like PgUp, PgDn, Home, End, Up, Down, etc.

**TABS**\
There are 10 static tabs.\
There are also menu buttons for the first 5 tabs.\
---------------------------------------------------\
**alt+(number 1->0)** - switch to tab x\
**ctrl+Tab** - switch to next tab\
**ctrl+shift+Tab** - switch to previous tab

**BROWSER**\
**alt+q** - quit browser\
**alt+f**, **F11** - fullscreen toggle\
**Esc** - exit fullscreen

**ZOOM**\
Zoom level and its controls are in the menu.\
---------------------------------------------------\
**ctrl+scroll** - zoom in/out\
**ctrl+equal** - zoom in\
**ctrl+minus** - zoom out\
**ctrl+0** - zoom reset

**FIND**\
Includes a prompt for the text.\
---------------------------------------------------\
**ctrl+f** - open the find prompt\
**ctrl+n** - next find match\
**ctrl+shift+n** - previous find match\
**Esc** - close find prompt & stop find

**BOOKMARKS, HISTORY, COOKIES**\
There's a star button for quick bookmarking.\
Each one has a menu item and they open an external window.\
Bookmarks are stored in ~/.config/xupric/bookmarks.db\
History and cookies are stored in ~/.cache/xupric/\
---------------------------------------------------\
**ctrl+b** - toggle bookmark

**DOWNLOADS**\
When download starts, a path prompt is shown.\
The download info and cancel button is in the menu.\
---------------------------------------------------\
**ctrl+s** - download current page

**DARK MODE**\
Dark mode looks a bit off here-n-there.\
There's a dark mode toggle button (per tab) next to the menu.\
If any webpage looks odd, then turn it on..\
---------------------------------------------------\
**ctrl+d** - toggle dark mode

**URL FILTERING**\
Clears urls with clearURLs filters.\
It produces almost the same result as with clearURLs.\
For some reason, when pasting urls into the uri entry,\
it doesn't filter correctly.\
By default, it is enabled. (config option)

**ADBLOCKING**\
The browser does not have adblocking built in.\
It's because I have Pi-Hole, which blocks most of the ads already.\
If you'd like to block ads, either:\
* block with /etc/hosts\
* or use: https://github.com/jun7/wyebadblock (haven't tried)\
(sandboxing is disabled, dir: ~/.config/xupric/extensions)

**FIRE BUTTON**\
The fire button next to the secondary uri entry,\
runs the cookie filters and clears all history.\
(exceptions are defined in the config)\
This was greatly inspired by DDG's Privacy Browser.

**DEBUG**\
There's a toggle debug button: menu->Help->Toggle Debug\
This gives out a ton of debugging messages in stdout.\
Also, if the browser crashes for any reason, be sure\
to run the browser through the terminal and see the messages.\
I wouldn't recommend enabling it for no reason.\
---------------------------------------------------\
**ctrl+shift+d** - toggle debugging

**MISC**\
**alt+s** - clear uri entry and focus\
**alt+shift+s** - clear secondary uri entry and focus\
**ctrl+i** - toggle webinspector

**QUICK**\
**ctrl+shift+alt+a** - load amazon.com\
**ctrl+shift+alt+d** - load discord.com\
**ctrl+shift+alt+e** - load ebay.com\
**ctrl+shift+alt+g** - load github.com\
**ctrl+shift+alt+i** - load twitter.com\
**ctrl+shift+alt+m** - load mail.zoho.com\
**ctrl+shift+alt+n** - load netflix.com\
**ctrl+shift+alt+o** - load odysee.com\
**ctrl+shift+alt+r** - load reddit.com\
**ctrl+shift+alt+t** - load trello.com\
**ctrl+shift+alt+w** - load wikipedia.org\
**ctrl+shift+alt+y** - load youtube.com

**WORKSPACES**\
Workspaces can be defined in the config.\
There are max 5 workspaces.\
---------------------------------------------------\
**ctrl+shift+alt+(number 1-5)** - load workspace x

# CONFIGURATION
The configuration directory is:\
**~/.config/xupric/**\
The config file, which holds a ton of options is:\
**~/.config/xupric/xupric.conf**\
The css styles used in xupric, are in:\
**~/.config/xupric/styles/app/**\
The shortcuts must be configured before compiling, in:\
**src/config.h**\
Xupric is styled using the **Tomorrow-Night** color theme.\
It was intended to not look like a gtk application.

**Custom css, js and certs depending on the uri:**\
The directories are ~/.config/xupric/(styles, scripts, certs)\
The filename must be the **domain** of the website.\
Make sure to pay attention to the naming!\
To match all uris, the filename must be *

# CACHE
The cache is stored in **~/.cache/xupric/**\
It is mostly managed by webkit2gtk.

# BUGS
If you encounter any bugs, feel free to open an issue.

# ABOUT
https://www.github.com/chm46e/xupric

# AUTHORS\
chm46e <chm46e@duck.com>




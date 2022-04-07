#!/bin/sh

# a quick copy script that copies the contents from .config to this project

test=$(pwd | grep src/sh)

if [ "$test" = "" ]; then
	printf "Not in the /src/sh/ directory. Aborted..."
	exit 1
fi

cp -f ~/.config/xupric/xupric.conf ../../
cp -f ~/.config/xupric/styles/main.css ../css/
cp -f ~/.config/xupric/styles/dark_mode.css ../css/
cp -f ~/.config/xupric/styles/scrollbar.css ../css/

printf "Update completed"

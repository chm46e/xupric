#!/bin/sh

# a quick copy script that copies the contents from .config to this project

test=$(pwd | grep src/sh)

if [ "$test" = "" ]; then
	printf "Not in the /src/sh/ directory. Aborted..."
	exit 1
fi

cp -f ~/.config/xupric/xupric.conf ../../
cp -f ~/.config/xupric/styles/app/main.css ../css/
cp -f ~/.config/xupric/styles/app/dark_mode.css ../css/
cp -f ~/.config/xupric/styles/app/scrollbar.css ../css/

printf "Update completed"

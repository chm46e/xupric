#!/bin/sh

# a quick copy script that copies the contents from .config to this project

test=$(pwd | grep src/sh)

if [ "$test" = "" ]; then
	printf "Not in the /src/sh/ directory. Aborted..."
	exit 1
fi

cp -f ~/.config/sulfer/sulfer.conf ../../
cp -f ~/.config/sulfer/styles/main.css ../css/
cp -f ~/.config/sulfer/styles/dark_mode.css ../css/
cp -f ~/.config/sulfer/styles/scrollbar.css ../css/

printf "Update completed"

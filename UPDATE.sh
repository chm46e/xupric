#!/bin/sh

test=$(ls -lah | grep xupric.conf)

if [ "$test" = "" ]; then
	printf "Not in the xupric/ directory. Aborted.."
	exit 1
fi

printf "Danger! This will overwrite all your changes in config/style files!\n"
printf "Soo it updates your local files to the new ones\n"
printf "If you'd like to cancel, press ctrl+c\n"
printf "To continue, press enter\n"

read choice
cp -f xupric.conf ~/.config/xupric/xupric.conf
cp -f src/css/main.css ~/.config/xupric/styles/app/main.css
cp -f src/css/dark_mode.css ~/.config/xupric/styles/app/dark_mode.css
cp -f src/css/scrollbar.css ~/.config/xupric/styles/app/scrollbar.css

printf "Update completed!"



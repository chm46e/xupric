#!/bin/sh

mkdir -p ~/.config/xupric/styles/app/

cd ../
cp -n xupric.conf ~/.config/xupric/
cp -n src/css/main.css ~/.config/xupric/styles/app/
cp -n src/css/dark_mode.css ~/.config/xupric/styles/app/
cp -n src/css/scrollbar.css ~/.config/xupric/styles/app/

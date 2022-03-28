#!/bin/sh

mkdir -p ~/.config/sulfer/styles/

cd ../
cp -n sulfer.conf ~/.config/sulfer/
cp -n src/css/main.css ~/.config/sulfer/styles/
cp -n src/css/dark_mode.css ~/.config/sulfer/styles/
cp -n src/css/scrollbar.css ~/.config/sulfer/styles/

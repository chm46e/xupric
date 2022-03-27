#!/bin/sh

# a quick copy script that copies the contents from .config to this project
# NB! make sure to execute it from src/sh/ directory!

cp -f ~/.config/sulfer/main.ui ../
cp -f ~/.config/sulfer/sulfer.conf ../../
cp -f ~/.config/sulfer/styles/main.css ../css/
cp -f ~/.config/sulfer/styles/dark_mode.css ../css/
cp -f ~/.config/sulfer/styles/scrollbar.css ../css/



#!/bin/sh

mkdir -p /usr/share/icons/hicolor/256x256/apps/
cp -f ../res/sulfer.png /usr/share/icons/hicolor/256x256/apps/sulfer.png
mkdir -p /usr/share/applications/
cp -f ../res/sulfer.desktop /usr/share/applications/sulfer.desktop

cp -n ../res/icons/sulfer_star_no.png /usr/share/icons/hicolor/scalable/apps/
cp -n ../res/icons/sulfer_star_yes.png /usr/share/icons/hicolor/scalable/apps/

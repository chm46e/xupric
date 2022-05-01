#!/bin/sh

mkdir -p /usr/share/icons/hicolor/256x256/apps/
cp -f ../res/xupric.png /usr/share/icons/hicolor/256x256/apps/xupric.png
mkdir -p /usr/share/applications/
cp -f ../res/xupric.desktop /usr/share/applications/xupric.desktop
mkdir -p /usr/local/man/man1/
cp -f ../res/xupric.1 /usr/local/man/man1/xupric.1

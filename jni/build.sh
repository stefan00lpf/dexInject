#!/bin/bash
ndk-build clean
ndk-build
adb push ../libs/armeabi-v7a/* /data/local/kk/
adb push config.json /data/local/kk/
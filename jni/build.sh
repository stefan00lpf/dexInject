#!/bin/bash
ndk-build clean
ndk-build
adb push ../libs/armeabi-v7a/libmyhook.so /data/local/kk/
adb push config.json /data/local/kk/
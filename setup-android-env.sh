#!/bin/bash

# create required folders
mkdir --parents android/sdk android/build assets include lib/armeabi-v7a lib/arm64-v8a lib/x86 lib/x86_64 src
cd android/build
mkdir --parents obj dex res/values src/com/raylib/game assets
mkdir --parents lib/armeabi-v7a lib/arm64-v8a lib/x86 lib/x86_64
mkdir --parents res/drawable-ldpi res/drawable-mdpi res/drawable-hdpi res/drawable-xhdpi
cd ../..


# install deps
cd android/sdk/cmdline-tools/bin
./sdkmanager --update --sdk_root=../..
./sdkmanager --install "build-tools;29.0.3" --sdk_root=../..
./sdkmanager --install "platform-tools" --sdk_root=../..
./sdkmanager --install "platforms;android-29" --sdk_root=../..
cd ../../../..


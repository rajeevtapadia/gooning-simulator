#!/bin/bash

set -e

# compile raylib
cd raylib/src
cp raylib.h ../../include
make clean
make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=arm ANDROID_API_VERSION=34
mv libraylib.a ../../lib/armeabi-v7a
make clean
make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=arm64 ANDROID_API_VERSION=34
mv libraylib.a ../../lib/arm64-v8a
make clean
make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=x86 ANDROID_API_VERSION=34
mv libraylib.a ../../lib/x86
make clean
make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=x86_64 ANDROID_API_VERSION=34
mv libraylib.a ../../lib/x86_64
make clean
cd ../..

# copy default icon
cp raylib/logo/raylib_36x36.png assets/icon_ldpi.png
cp raylib/logo/raylib_48x48.png assets/icon_mdpi.png
cp raylib/logo/raylib_72x72.png assets/icon_hdpi.png
cp raylib/logo/raylib_96x96.png assets/icon_xhdpi.png

# generate a kay for signing APK
cd android
keytool -genkeypair -validity 1000 -dname "CN=raylib,O=Android,C=ES" -keystore raylib.keystore -storepass 'raylib' -keypass 'raylib' -alias projectKey -keyalg RSA
cd ..

cp -v ./NativeLoader.java android/build/src/com/raylib/game/
cp -v ./AndroidManifest.xml android/build/

DIR_MAKEFILE := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
APK_FILE = $(DIR_MAKEFILE)/src/raylib_game.apk
APP_NAME = com.raylib.rgame

.PHONY: build build-android 

build:
	cmake --build build

run:
	make build
	cd bin && tetris-game.exe

build-android:
	cd src && make clean && $(MAKE) PLATFORM=PLATFORM_ANDROID

run-android:
	make build-android
	adb install $(APK_FILE)
	adb shell am start -n $(APP_NAME)/.NativeLoader

clean:
	cd src && $(MAKE) clean
	adb uninstall $(APP_NAME)
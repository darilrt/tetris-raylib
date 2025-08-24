DIR_MAKEFILE := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
APK_FILE = $(DIR_MAKEFILE)/raylib_game.apk
APP_NAME = com.raylib.rgame

.PHONY: build build-android 

build:
	cmake --build build

run:
	make build
	cd bin && tetris-game.exe

build-android:
	make -f Makefile.pre clean && $(MAKE) -f Makefile.pre PLATFORM=PLATFORM_ANDROID

run-android:
	make build-android
	adb install $(APK_FILE)
	adb shell am start -n $(APP_NAME)/.NativeLoader

clean:
	cmake --build build --target clean
# Use aarch64-linux-gnu-gcc if the host is not aarch64
ifeq ($(shell uname -m),aarch64)
CC=gcc
else
CC=/opt/aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
endif

CFLAGS= -Iworkspace/include -Wall
LDFLAGS=-lSDL2 -lSDL2_ttf -lSDL2_image -lm
PROJECT_NAME=LedController

.PHONY: all clean deps 

all: led_controller

led_controller:
	mkdir -p build
	$(CC) $(CFLAGS) workspace/src/led_controller.c workspace/src/sdl_base.c -o build/$@ $(LDFLAGS)


package: 
	# Create general project package
	mkdir -p release/${PROJECT_NAME}
	cp scripts/led-controller-launch.sh release/${PROJECT_NAME}/launch.sh
	cp -r workspace/assets/ build/* workspace/config_files/* release/${PROJECT_NAME}
	chmod -R 777 release/${PROJECT_NAME}

	# Create led_controller project package for MinUI
	mkdir -p release/${PROJECT_NAME}.pak
	cp scripts/led-controller-launch.sh release/${PROJECT_NAME}.pak/launch.sh
	cp -r workspace/assets/ workspace/config_files/* build/* release/${PROJECT_NAME}.pak
	chmod -R 777 release/${PROJECT_NAME}.pak

clean:
	rm -rf build release

deps:
	apt update
	apt install sudo
	sudo apt-get update
	sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
	sudo dev_scripts/setup-toolchain.sh
	sudo dev_scripts/setup-env.sh





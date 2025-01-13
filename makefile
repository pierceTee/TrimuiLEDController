# Compiler and flags for host
CC = gcc
ARCH := $(shell uname -m)
BUILD_DIR = build/$(ARCH)
RELEASE_DIR = release/$(ARCH)
CFLAGS = -I/usr/include/SDL2 -Iworkspace/include -Wall
LDFLAGS = -L/usr/lib -lSDL2 -lSDL2_ttf -lSDL2_image -lm -g

# General flags
PROJECT_NAME=LedController

.PHONY: all clean deps

all: led_controller

led_controller:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/led_controller workspace/src/led_controller.c workspace/src/sdl_base.c $(LDFLAGS)

package: all
	mkdir -p $(RELEASE_DIR)

	# Create general project package
	mkdir -p $(RELEASE_DIR)/$(PROJECT_NAME)
	mkdir -p $(RELEASE_DIR)/$(PROJECT_NAME).pak

	# Copy the launch script to the release directory
	cp -r workspace/scripts/led-controller-launch.sh $(RELEASE_DIR)/$(PROJECT_NAME)/launch.sh
	cp -r workspace/scripts/led-controller-launch-minui.sh $(RELEASE_DIR)/$(PROJECT_NAME).pak/launch.sh

	# Copy the service scripts to the release directory
	cp workspace/scripts/service/* $(RELEASE_DIR)/$(PROJECT_NAME)
	cp workspace/scripts/service/* $(RELEASE_DIR)/$(PROJECT_NAME).pak
	
	# Copy the assets, config files, and build files to the release directory
	cp -r workspace/assets/ $(BUILD_DIR)/* workspace/config_files/* $(RELEASE_DIR)/$(PROJECT_NAME)
	cp -r workspace/assets/ $(BUILD_DIR)/* workspace/config_files/* $(RELEASE_DIR)/$(PROJECT_NAME).pak

	# Make the release directory executable by all users
	chmod -R u+rx,g+rx,o+rx $(RELEASE_DIR)/$(PROJECT_NAME)
	chmod -R u+rx,g+rx,o+rx $(RELEASE_DIR)/$(PROJECT_NAME).pak

clean:
	rm -rf build release

deps:
	# Ensure both target architecture's package libaries are available
	sudo apt-get update

	# Install SDL libraries for both architectures
	sudo apt install -y libsdl2-dev:amd64 libsdl2-ttf-dev:amd64 libsdl2-image-dev:amd64
	sudo apt install -y libsdl2-dev:arm64 libsdl2-ttf-dev:arm64 libsdl2-image-dev:arm64

	# link the x86_64 libraries to the correct location 
	sudo ln -s /usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0 /usr/lib/x86_64-linux-gnu/libSDL2.so
	sudo ln -s /usr/lib/x86_64-linux-gnu/libSDL2_ttf-2.0.so.0 /usr/lib/x86_64-linux-gnu/libSDL2_ttf.so
	sudo ln -s /usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so.0 /usr/lib/x86_64-linux-gnu/libSDL2_image.so

	# Install the cross-compilation tools
	sudo apt-get install -y gcc g++
	sudo apt-get install -y gcc-x86-64-linux-gnu g++-x86-64-linux-gnu

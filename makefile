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
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/led_controller workspace/src/led_controller_common.c workspace/src/led_controller.c workspace/src/sdl_base.c  $(LDFLAGS)
	chmod -R a+rwx $(BUILD_DIR)

package: all
	mkdir -p $(RELEASE_DIR)

	# Create general project package
	mkdir -p $(RELEASE_DIR)/$(PROJECT_NAME).pak/scripts

	# Copy the launch script to the release directory
	cp -r workspace/scripts/launch/launch.sh $(RELEASE_DIR)/$(PROJECT_NAME).pak/launch.sh

	# Copy the service scripts to the release directory
	cp workspace/scripts/runtime/* $(RELEASE_DIR)/$(PROJECT_NAME).pak/scripts
	
	# Copy the assets, service files, config files, and build files to the release directory
	cp -r workspace/assets/ workspace/service/ $(BUILD_DIR)/* workspace/config_files/* $(RELEASE_DIR)/$(PROJECT_NAME).pak

	# Make the release directory executable by all users
	chmod -R a+rwx $(RELEASE_DIR)

release: package
	# Create the final release package
	mkdir -p $(RELEASE_DIR)/github	
	cp github/readme.txt $(RELEASE_DIR)

	# zip up the project
	cd $(RELEASE_DIR) && zip -r github/$(PROJECT_NAME).zip . -x "github/*"
	cd - 
	# zip up all the source code
	zip -r $(RELEASE_DIR)/github/source_code.zip . -x "release/*" "build/*" ".git/*" ".github/*"
	chmod -R a+rwx $(RELEASE_DIR)/github

clean:
	rm -rf build release

deps:
	# Ensure both target architecture's package libaries are available
	sudo apt-get update

	# Install SDL libraries for both architectures
	sudo apt install -y libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev

	# Install the cross-compilation tools
	sudo apt-get install -y gcc g++ zip

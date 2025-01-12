# Compiler and flags for ARM64
ARM_64_CC=aarch64-linux-gnu-gcc
ARM_64_BUILD_DIR=build/arm64
ARM_64_RELEASE_DIR=release/arm64
ARM_64_CFLAGS= -I/usr/include/aarch64-linux-gnu/SDL2 -Iworkspace/include -Wall
ARM_64_LDFLAGS= -L/usr/lib/aarch64-linux-gnu -lSDL2 -lSDL2_ttf -lSDL2_image -lm -g

# Compiler and flags for x86
X86_CC=x86_64-linux-gnu-gcc
X86_BUILD_DIR=build/x86
X86_RELEASE_DIR=release/x86
X86_CFLAGS = -I/usr/include/x86_64-linux-gnu/SDL2 -Iworkspace/include -Wall
X86_LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lSDL2 -lSDL2_ttf -lSDL2_image -lm -g

# General flags
PROJECT_NAME=TrimUILedController

.PHONY: all clean deps

all: led_controller_arm64 led_controller_x86

led_controller_arm64:
	mkdir -p ${ARM_64_BUILD_DIR}
	$(ARM_64_CC) $(ARM_64_CFLAGS) -o ${ARM_64_BUILD_DIR}/led_controller workspace/src/led_controller.c workspace/src/sdl_base.c $(ARM_64_LDFLAGS)

led_controller_x86:
	mkdir -p ${X86_BUILD_DIR}
	$(X86_CC) $(X86_CFLAGS) -o ${X86_BUILD_DIR}/led_controller workspace/src/led_controller.c workspace/src/sdl_base.c $(X86_LDFLAGS)

package: all
	mkdir -p ${ARM_64_RELEASE_DIR}
	mkdir -p ${X86_RELEASE_DIR}

	# Create general project package
	mkdir -p ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}
	mkdir -p ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}.pak
	mkdir -p ${X86_RELEASE_DIR}/${PROJECT_NAME}

	# Copy the launch script to the release directory
	cp -r workspace/scripts/led-controller-launch.sh ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}/launch.sh
	cp -r workspace/scripts/led-controller-launch-minui.sh ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}.pak/launch.sh
	cp -r workspace/scripts/led-controller-launch.sh ${X86_RELEASE_DIR}/${PROJECT_NAME}/launch.sh

	# Copy the service scripts to the release directory
	cp workspace/scripts/service/* ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}
	cp workspace/scripts/service/* ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}.pak
	cp workspace/scripts/service/* ${X86_RELEASE_DIR}/${PROJECT_NAME}
	
	# Copy the assets, config files, and build files to the release directory
	cp -r workspace/assets/ ${ARM_64_BUILD_DIR}/* workspace/config_files/* ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}
	cp -r workspace/assets/ ${ARM_64_BUILD_DIR}/* workspace/config_files/* ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}.pak
	cp -r workspace/assets/ ${X86_BUILD_DIR}/* workspace/config_files/* ${X86_RELEASE_DIR}/${PROJECT_NAME}

	# Make the release directory executable by all users
	chmod -R u+rx,g+rx,o+rx ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}
	chmod -R u+rx,g+rx,o+rx ${ARM_64_RELEASE_DIR}/${PROJECT_NAME}.pak
	chmod -R u+rx,g+rx,o+rx ${X86_RELEASE_DIR}/${PROJECT_NAME}

clean:
	rm -rf build release

deps:
	# Ensure both target architecture's package libraries are available
	sudo dpkg --add-architecture amd64
	sudo dpkg --add-architecture arm64

	sudo apt-get update	
	# Install SDL libraries for both architectures
	sudo apt install -y libsdl2-dev:amd64 libsdl2-ttf-dev:amd64 libsdl2-image-dev:amd64
	sudo apt install -y libsdl2-dev:arm64 libsdl2-ttf-dev:arm64 libsdl2-image-dev:arm64

	# Link the x86_64 libraries to the correct location
	sudo ln -s /usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0 /usr/lib/x86_64-linux-gnu/libSDL2.so
	sudo ln -s /usr/lib/x86_64-linux-gnu/libSDL2_ttf-2.0.so.0 /usr/lib/x86_64-linux-gnu/libSDL2_ttf.so
	sudo ln -s /usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so.0 /usr/lib/x86_64-linux-gnu/libSDL2_image.so

	# Install the cross-compilation tools
	sudo apt-get install -y gcc g++
	sudo apt-get install -y gcc-x86-64-linux-gnu gcc-aarch64-linux-gnu
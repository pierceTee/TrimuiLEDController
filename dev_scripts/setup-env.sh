#!/bin/bash
######################################################################################################
# Taken from shauninman's union-tg3040-toolchain: https://github.com/shauninman/union-tg3040-toolchain
######################################################################################################

# This is meant to be run within the container.

# If we're already on an aarch64 system, we set our compiler flags to defaults and exit.
TOOLCHAIN_ARCH=`uname -m`
if [ "$TOOLCHAIN_ARCH" = "aarch64" ]; then
	export CROSS_COMPILE=/usr/bin/aarch64-linux-gnu-   
	export PREFIX=/usr 
else
	# Otherwise, we need to explicitly path to the target compiler.
	export PATH="/opt/aarch64-linux-gnu/aarch64-linux-gnu/bin:${PATH}:/opt/aarch64-linux-gnu/aarch64-linux-gnu/libc/bin"
	export CROSS_COMPILE=/opt/aarch64-linux-gnu/bin/aarch64-linux-gnu-
	export PREFIX=/opt/aarch64-linux-gnu/aarch64-linux-gnu/libc/usr
fi
export UNION_PLATFORM=tg3040

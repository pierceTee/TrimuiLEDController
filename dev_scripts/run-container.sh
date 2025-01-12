#!/bin/sh
# Run from the root of the projects

# Function to check if a Docker container exists
container_exists() {
    docker ps -a --format '{{.Names}}' | grep -Eq "^$1\$"
}

# Container name
CONTAINER_NAME="arm64-tg3040-toolchain"

# Check if the container exists and build it if it doesn't
if ! container_exists "$CONTAINER_NAME"; then
    echo "Container $CONTAINER_NAME does not exist. Building the toolchain..."
    cd toolchains/arm64-tg3040-toolchain
    make .build
    cd -
fi

# Run the QEMU emulator and allows it to handle ARM64 binaries
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes

# Allow local connections to X server
xhost +local:root

# Run the Docker container
docker run -it --privileged \
    --platform linux/arm64 \
    -e DISPLAY=$DISPLAY \
    -e XDG_RUNTIME_DIR=/tmp \
    -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
    -v $XAUTHORITY:/root/.Xauthority:rw \
    -v $(pwd)/:/root/workspace \
    --device /dev/dri \
    $CONTAINER_NAME

# Revoke local connections to X server
xhost -local:root
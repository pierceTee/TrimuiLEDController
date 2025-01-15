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

# Check for the "release" parameter
if [ "$1" = "release" ]; then
    MAKE_COMMAND="make && make release"
else
    MAKE_COMMAND="make && make package"
fi

# Run the make commands
eval $MAKE_COMMAND

# Run the Docker container and execute the make commands
docker run --rm --privileged \
    --platform linux/arm64 \
    -v $(pwd)/:/root/workspace \
    --device /dev/dri \
    $CONTAINER_NAME sh -c "cd /root/workspace && $MAKE_COMMAND"

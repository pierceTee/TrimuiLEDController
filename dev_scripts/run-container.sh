xhost +local:root
docker run -it --privileged \
    -e DISPLAY=$DISPLAY \
    -e XDG_RUNTIME_DIR=/tmp \
    -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
    -v $XAUTHORITY:/root/.Xauthority:rw \
    -v $(pwd)/:/root/workspace \
    --device /dev/dri \
    arm64-tg3040-toolchain
xhost -local:root
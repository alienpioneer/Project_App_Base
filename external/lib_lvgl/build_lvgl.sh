#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

MODE="host"   # host | target | yocto

PROJECT_ROOT=$(pwd)
LVGL_SRC_DIR="$PROJECT_ROOT/lvgl"
LVGL_BUILD_DIR="$LVGL_SRC_DIR/build"
LVGL_INSTALL_DIR="$PROJECT_ROOT/libs"

USE_GPU="ON"
USE_SDL_DRAW="ON"
BUILD_TYPE="Debug"

TOOLCHAIN_FILE="../../cmake/toolchain-rpi.cmake"

for arg in "$@"
do
    case "$arg" in
        "host")     MODE="host" ;;
        "target")   MODE="target";;
        "sdlsw")    USE_SDL_DRAW="OFF" ;;
        "cpu")      USE_GPU="OFF" ;;
        "debug")    BUILD_TYPE="Debug" ;;
        "rel")      BUILD_TYPE="Release";;
        "reldeb")   BUILD_TYPE="RelWithDebInfo";;
        "min")      BUILD_TYPE="MinSizeRel";;
        "h"|"--help") 
            echo "Usage: ./build.sh [host|target] [debug|rel|reldeb|min] [cpu|sdlsw]"
            echo "sdlsw DISABLES the SDL_DRAW"
            exit 0 ;;
        *) echo "Unknown argument: $arg (skipping)" ;;
    esac
done

echo "--- Starting LVGL Static Build ---"
echo "------------------------------------------"
echo "BUILD CONFIGURATION:"
echo "  MODE:        $MODE"
echo "  BUILD TYPE:  $BUILD_TYPE"
echo "  Use GPU:     $USE_GPU"
echo "  SDL DRAW:    $USE_SDL_DRAW"
echo "------------------------------------------"

# Clean up previous build
if [ -d "$LVGL_BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -rf "$LVGL_BUILD_DIR"
fi

mkdir -p "$LVGL_BUILD_DIR"

if [ "$MODE" = "target" ]; then

    cmake -S "$SRC_DIR" -B "$LVGL_BUILD_DIR" \
        -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
        -DLVGL_INSTALL_DIR="$LVGL_INSTALL_DIR" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DUSE_GPU="$USE_GPU" \
        -DUSE_SDL_DRAW="$USE_SDL_DRAW"

else

    cmake -S . -B "$LVGL_BUILD_DIR" \
        -DLVGL_INSTALL_DIR="$LVGL_INSTALL_DIR" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DUSE_GPU="$USE_GPU" \
        -DUSE_SDL_DRAW="$USE_SDL_DRAW"

fi

echo "---------> LVGL configure completed."

cmake --build "$LVGL_BUILD_DIR" --target lvgl --parallel 8 --clean-first

echo "--- LVGL Build Complete ---"
echo "Library installed to: $LVGL_INSTALL_DIR"

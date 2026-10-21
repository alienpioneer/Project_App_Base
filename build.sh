#!/bin/sh
set -e

MODE="host"   # host | target
SCOPE="app"   # app | resources

BUILD_TYPE="RelWithDebInfo"
BUILD_DIR_BASE="build"
LOG_LEVEL="LOG_LEVEL_DEBUG" 

BUILD_RESOURCES="OFF"
RES_IMAGES="OFF"
RES_AUDIO="OFF"
RES_FONTS="OFF"
RES_VECTOR="OFF"

USE_GPU="ON"

RUN=false
DEPLOY=false
# Must syncronize with DEPLOY_DIR from CMakeLists
DEPLOY_DIR=./deploy/target

UNIT_OPT="OFF"
STATIC_OPT="OFF"
CLEAN_OPT="OFF"

PYTHON_PATH=$(which python3)
TARGET="strutu@192.168.1.32"
TOOLCHAIN_FILE="cmake/toolchain-rpi.cmake"


for arg in "$@"
do
    case "$arg" in
        host)    MODE="host" ; DEPLOY_DIR="./deploy/host" ;;
        target)  MODE="target";;

        clean)   CLEAN_OPT="ON";;

        unit)    MODE="host"; UNIT_OPT="ON"; DEPLOY_DIR="./deploy/host" ;;
        static)  MODE="host"; STATIC_OPT="ON"; DEPLOY_DIR="./deploy/host" ;;

        cpu)     USE_GPU="OFF";;

        debug)   BUILD_TYPE="Debug" ; LOG_LEVEL="LOG_LEVEL_DEBUG" ;;
        release) BUILD_TYPE="Release" ; LOG_LEVEL="LOG_LEVEL_INFO" ;;
        reldeb)  BUILD_TYPE="RelWithDebInfo" ; LOG_LEVEL="LOG_LEVEL_DEBUG" ;;
        min)     BUILD_TYPE="MinSizeRel" ; LOG_LEVEL="LOG_LEVEL_INFO";;

        img)     SCOPE="resources"; RES_IMAGES="ON";;
        audio)   SCOPE="resources"; RES_AUDIO="ON";;
        fonts)   SCOPE="resources"; RES_FONTS="ON";;
        vector)  SCOPE="resources"; RES_VECTOR="ON";;
        resall)  SCOPE="resources"; RES_IMAGES="ON"; RES_AUDIO="ON"; RES_FONTS="ON"; RES_VECTOR="ON";;

        run)     RUN=true ; DEPLOY_DIR="./deploy/host";;
        deploy)  DEPLOY=true;;

        h|--help)
            echo "Usage:"
            echo "./build.sh [unit] [host|target] [debug|release|reldeb|min] [run] [deploy] [img|audio|fonts|vector|resall] [clean] [cpu]"
            exit 0;;
        *)
            echo "Unknown argument: $arg (skipping)";;
    esac
done

# Change build directory depending on operation
BUILD_DIR="$BUILD_DIR_BASE"
[ "$MODE" = "target" ] && BUILD_DIR="${BUILD_DIR}-target"
[ "$SCOPE" = "resources" ] && BUILD_DIR="${BUILD_DIR}-res"

SRC_DIR="."
[ "$SCOPE" = "resources" ] && SRC_DIR="./resources"

if [ "$SCOPE" = "resources" ]; then
    EXTRA_ARGS="-DRES_AUDIO=$RES_AUDIO -DRES_IMAGES=$RES_IMAGES -DRES_FONTS=$RES_FONTS -DRES_VECTOR=$RES_VECTOR"
    TOOLCHAIN_FILE="../$TOOLCHAIN_FILE"
else
    EXTRA_ARGS=""
fi

# Clean
if [ "$CLEAN_OPT" = "ON" ] && [ -d "$BUILD_DIR" ]; then
    echo "--- Clean project folder ---"
    rm -rf "$BUILD_DIR"
fi

echo "------------------------------------------"
echo "MODE:        $MODE"
echo "SCOPE:       $SCOPE"
echo "BUILD TYPE:  $BUILD_TYPE"
echo "USE GPU:     $USE_GPU"
echo "BUILD DIR:   $BUILD_DIR"
echo "UNIT TESTS:  $UNIT_OPT"
echo "STATIC TEST: $STATIC_OPT"
echo "------------------------------------------"

# CONFIGURE

if [ "$MODE" = "target" ]; then

    if [ "$BUILD_TYPE" = "Release" ]; then
        export CXXFLAGS="-O3 -g0 -DNDEBUG"
        export CFLAGS="-O3 -g0 -DNDEBUG"
    fi

    cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
        -DBUILD_PYTHON="$PYTHON_PATH" \
        -DBUILD_RESOURCES="$BUILD_RESOURCES" \
        -DUSE_GPU="$USE_GPU" \
        -DLOG_LEVEL="$LOG_LEVEL" \
        $EXTRA_ARGS

else
    cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DRUN_UNIT_TESTS="$UNIT_OPT" \
        -DBUILD_RESOURCES="$BUILD_RESOURCES" \
        -DUSE_GPU="$USE_GPU" \
        -DLOG_LEVEL="$LOG_LEVEL" \
        -DRUN_STATIC_ANALYSIS="$STATIC_OPT" \
        $EXTRA_ARGS
fi

cmake --build "$BUILD_DIR" --target app --parallel 8
# cmake --build "$BUILD_DIR" --parallel 8 --clean-first

# ---- Actions ----

if $RUN; then
    if [ "$MODE" = "host" ] && [ "$SCOPE" = "app" ]; then
        cd "$DEPLOY_DIR"
        # pwd
        ./app
    else
        echo "Run is only valid for host. Use deploy for target."
    fi
fi

if $DEPLOY; then
    if [ "$MODE" = "target" ] && [ "$SCOPE" = "app" ]; then
        # Stop the service to release file locks and hardware resources
        # ssh $TARGET "systemctl stop app"
        
        # Copy the updated binary and configuration files
        scp "$DEPLOY_DIR/app" $TARGET:~/
        scp "$DEPLOY_DIR/app.ini" $TARGET:~/
        
        # Restart the application under systemd management
        # ssh $TARGET "systemctl start app && journalctl -u app -f"
    else
        echo "deploy only valid for target app build"
    fi
fi

if [ "$UNIT_OPT" = "ON" ]; then
    echo "--- Running Unit Tests ---"
    cmake --build "$BUILD_DIR" --target run_unit_tests
fi

if [ "$STATIC_OPT" = "ON" ]; then
    echo "--- Running Static Analysis ---"
    cmake --build "$BUILD_DIR" --target test_cppcheck
fi

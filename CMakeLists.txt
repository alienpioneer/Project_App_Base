# @file CMakeLists.txt
# @author Alexandru ALEXANDRESCU
# All rights reserved.

cmake_minimum_required(VERSION 3.22)

project(app_project)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME app)

# ----------- SETUPS

# Global warnings (safe)
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)

# Set global log level
if(NOT DEFINED LOG_LEVEL OR LOG_LEVEL STREQUAL "")
    add_compile_definitions(LOG_COMPILED_LEVEL=LOG_LEVEL_DEBUG)
else()
    add_compile_definitions(LOG_COMPILED_LEVEL=${LOG_LEVEL})
endif()

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g1 -DNDEBUG")

# ----------- OPTIONS

option(USE_GPU "Enable GPU rendering" ON)

# ----------- PATHS

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(DEPLOY_DIR ${CMAKE_SOURCE_DIR}/deploy)
set(RUN_TESTS_DIR ${CMAKE_BINARY_DIR}/tests)

# ----------- LIBRARIES

# IMPORTANT - ADD HERE THE LIBRARY PROJECT LOCATION
set(CORE_PATH "/home/alex/__DEV__/Project_Core/deploy")

if(NOT IS_DIRECTORY "${CORE_PATH}")
    message(FATAL_ERROR "Not a valid directory for CORE project: ${CORE_PATH}")
endif()

# ----------- DEPENDENCIES

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)
find_package(OpenSSL REQUIRED)

if(CMAKE_CROSSCOMPILING)

    message(STATUS "CMAKE_CROSSCOMPILING = ${CMAKE_CROSSCOMPILING}")
    
    message(STATUS "Mode: DRM/KMS Hardware")

    set(PLATFORM "target")

    pkg_check_modules(DRM REQUIRED libdrm)
    pkg_check_modules(GBM REQUIRED gbm)
    pkg_check_modules(EGL REQUIRED egl)
    pkg_check_modules(GLESV2 REQUIRED glesv2)

    add_definitions(-DUSE_DRM)

else()
    message(STATUS "Mode: SDL2 Simulator")

    set(PLATFORM "host")

    find_package(SDL2 REQUIRED)
endif()

set(LIB_CORE_PATH     "${CORE_PATH}/${PLATFORM}")
set(LIB_AUDIO_PATH    "${EXTERNAL_DIR}/lib_audiores")

set(LIB_CORE                "${LIB_CORE_PATH}/libcore.a")
set(LVGL_LIB                "${EXTERNAL_DIR}/lib_lvgl/libs/${PLATFORM}/liblvgl.a")
set(IMG_RESOURCES_LIB       "${EXTERNAL_DIR}/lib_lvgl/libs/${PLATFORM}/libimages.a")
set(FONTS_RESOURCES_LIB     "${EXTERNAL_DIR}/lib_lvgl/libs/${PLATFORM}/libfonts.a")
set(VECTOR_RESOURCES_LIB    "${EXTERNAL_DIR}/lib_lvgl/libs/${PLATFORM}/libvector.a")
set(AUDIO_RESOURCES_LIB     "${LIB_AUDIO_PATH}/${PLATFORM}/libaudiores.a")

# Build settings

set(APP_SOURCES 
    src/main.cpp

    src/managers/DataManager.cpp
    src/managers/ConfigManager.cpp
    
    src/data/IDataWriteConfig.cpp
    src/data/IDataWriteUIService.cpp
    src/data/IDataReadUIService.cpp
    src/data/IDataReadAudioService.cpp
    src/data/IDataReadBacklightService.cpp
    src/data/IDataReadAccessService.cpp
       
    src/hal/display/LvglPlatform.cpp
    src/hal/backlight/Backlight.cpp

    src/services/UIService.cpp
    src/services/AudioService.cpp
    src/services/BacklightService.cpp
    src/services/AccessService.cpp

    src/ui/components/ScreenNavigator.cpp
    src/ui/components/StyleManager.cpp
    src/ui/components/UIComponents.cpp
    src/ui/components/UIUtils.cpp
    src/ui/components/UIWidgets.cpp

    src/ui/screens/IBaseScreen.cpp
    src/ui/screens/IBasePopup.cpp
    src/ui/screens/MainScreen.cpp
    src/ui/screens/SettingsScreen.cpp
    src/ui/screens/AuthPopup.cpp

    src/ui/widgets/Dashboard.cpp

)

set(APP_COMMON_INCLUDE
    # The Event types are expecting no include path
    # If moved into a folder like /events must add "${CMAKE_CURRENT_SOURCE_DIR}/include/events"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/lib_lvgl/lvgl/include"
    "${CORE_PATH}/include"
    "${LIB_AUDIO_PATH}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
    ${ALSA_INCLUDE_DIRS}
)

add_executable(${TARGET_NAME} ${APP_SOURCES})

if(BUILD_RESOURCES)

    set(STANDALONE_BUILD OFF)

    include(cmake/lvgl-resources.cmake)
    include(cmake/audio-resources.cmake)

    if(NOT DEFINED AUDIO_LIB_NAME OR NOT DEFINED IMAGES_LIB_NAME)
        message(FATAL_ERROR "Undefined AUDIO_LIB_NAME or IMAGES_LIB_NAME")
    endif()

    add_dependencies(${TARGET_NAME} ${AUDIO_LIB_NAME} ${IMAGES_LIB_NAME})

else()
    set(_unused_build_python "${BUILD_PYTHON}")
endif()

if(CMAKE_CROSSCOMPILING)

    set(GPU_DEFINES "")

    if(USE_GPU)
        list(APPEND GPU_DEFINES
            LV_USE_OPENGLES=1
            LV_USE_DRAW_OPENGLES=1
            LV_LINUX_DRM_USE_EGL=1
       )
    endif()

    target_compile_definitions(${TARGET_NAME} PRIVATE 
        LV_CONF_PATH="${CMAKE_CURRENT_SOURCE_DIR}/external/lib_lvgl/lv_conf.h"
        LV_USE_SDL=0
        LV_USE_DRAW_SDL=0
        LV_USE_LINUX_DRM=1
        ${GPU_DEFINES}
    )

    target_include_directories(${TARGET_NAME} PRIVATE 
        ${APP_COMMON_INCLUDE} 
        ${DRM_INCLUDE_DIRS}
        ${GBM_INCLUDE_DIRS}
        ${EGL_INCLUDE_DIRS}
    )

    target_link_libraries(${TARGET_NAME} PRIVATE
        ${LVGL_LIB}
        ${LIB_CORE}
        ${DRM_LIBRARIES}
        ${GBM_LIBRARIES}
        ${EGL_LIBRARIES}
        ${GLESV2_LIBRARIES}
        ${ALSA_LIBRARIES} 
        m pthread 
        ${IMG_RESOURCES_LIB}
        ${AUDIO_RESOURCES_LIB}
        ${FONTS_RESOURCES_LIB}
        ${VECTOR_RESOURCES_LIB}
        OpenSSL::Crypto
    )

else()

    set(SDL_DRAW_VALUE 0)

    if(USE_GPU)
        set(SDL_DRAW_VALUE 1)
    endif()
    
    target_compile_definitions(${TARGET_NAME} PRIVATE 
        LV_CONF_PATH="${CMAKE_CURRENT_SOURCE_DIR}/external/lib_lvgl/lv_conf.h" 
        LV_USE_SDL=1
        LV_USE_DRAW_SDL=${SDL_DRAW_VALUE}
        LV_USE_LINUX_DRM=0
    )

    target_include_directories(${TARGET_NAME} PRIVATE
        ${APP_COMMON_INCLUDE}
    )

    target_link_libraries(${TARGET_NAME} PRIVATE
        ${LVGL_LIB}
        ${LIB_CORE}
        ${SDL2_LIBRARIES}
        ${ALSA_LIBRARIES}
        m pthread
        ${IMG_RESOURCES_LIB}
        ${AUDIO_RESOURCES_LIB}
        ${FONTS_RESOURCES_LIB}
        ${VECTOR_RESOURCES_LIB}
        OpenSSL::Crypto
    )
endif()

# ==================================== INSTALLATION ===================================

if(NOT CODE_COVERAGE )
    # Install to deploy directory
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPLOY_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}/${PLATFORM}"
        COMMAND ${CMAKE_COMMAND} -E copy ${TARGET_NAME} "${DEPLOY_DIR}/${PLATFORM}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/config/* "${DEPLOY_DIR}/${PLATFORM}"
    
        COMMENT "Installing ${TARGET_NAME} ..."
    )
endif()

# ==================================== TESTS ===================================

if(NOT CMAKE_CROSSCOMPILING)

    if (RUN_UNIT_TESTS)
        add_subdirectory(tests)
    endif()
    
    if(RUN_STATIC_ANALYSIS)
        # Try to find cppcheck in PATH
        find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)

        if(CPPCHECK_EXECUTABLE)
            message(STATUS "cppcheck found: ${CPPCHECK_EXECUTABLE}")

            add_custom_target(test_cppcheck
                COMMAND ${CPPCHECK_EXECUTABLE} 
                        --project=${CMAKE_BINARY_DIR}/compile_commands.json
                        -i "${CMAKE_CURRENT_SOURCE_DIR}/external/lib_lvgl"
                        -i "${CORE_PATH}/include"
                        --suppress=*:*external/lib_lvgl/* # Suppress all warnings from lib_lvgl headers
                        --suppress=*:*${CORE_PATH}/include/* # Suppress all warnings from core headers
                        --enable=all 
                        --inconclusive 
                        --inline-suppr
                        --std=c++17 
                        --suppress=missingIncludeSystem
                        --suppress=unusedFunction
                        #--suppress=functionConst
                        #--suppress=functionStatic

                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Running cppcheck static analysis"
                VERBATIM
            )

        else()
            message(STATUS "cppcheck not found. Static analysis target will be skipped.")
        endif()
    endif()

endif()


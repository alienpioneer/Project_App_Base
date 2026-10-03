cmake_minimum_required(VERSION 3.22)
project(lvgl_resources_lib)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(STANDALONE_BUILD ON)

if(CMAKE_CROSSCOMPILING)
    set(LVGL_RESOURCE_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../external/lib_lvgl/libs/target")
else()
    set(LVGL_RESOURCE_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../external/lib_lvgl/libs/host")
endif()

if (RES_IMAGES)
    include(../cmake/lvgl-images.cmake)
endif()

if(RES_AUDIO)
    include(../cmake/audio-resources.cmake)
endif()

if (RES_FONTS)
    include(../cmake/lvgl-fonts.cmake)
endif()

if (RES_VECTOR)
    include(../cmake/lvgl-vector-raster.cmake)
endif()

if (NOT RES_IMAGES AND NOT RES_AUDIO AND NOT RES_FONTS AND NOT RES_VECTOR)
    message(FATAL_ERROR "No resource build was mentioned !")
endif()

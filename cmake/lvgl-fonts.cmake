# @file CMakeLists.txt
# @author Alexandru ALEXANDRESCU
# All rights reserved.
#
# Fonts compilation for LVGL

cmake_minimum_required(VERSION 3.22)

if(NOT LVGL_RESOURCE_INSTALL_DIR)
    message(FATAL_ERROR "LVGL_RESOURCE_INSTALL_DIR not found !")
endif()

set(FONTS_LIB_NAME fonts)

set(FONTS_INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts")
set(LV_CONFIG_PATH "${CMAKE_SOURCE_DIR}/external/lib_lvgl/lv_conf.h")
set(LV_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/lib_lvgl")

if(STANDALONE_BUILD)
    set(FONTS_INPUT_DIR  ${CMAKE_SOURCE_DIR}/fonts)
    set(LV_CONFIG_PATH "${CMAKE_SOURCE_DIR}/../external/lib_lvgl/lv_conf.h")
    set(LV_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../external/lib_lvgl")
endif()

find_package(PkgConfig REQUIRED)

if(CMAKE_CROSSCOMPILING)
    # if(NOT DEFINED ENV{SDKTARGETSYSROOT})
    #     message(FATAL_ERROR
    #         "Cross-compiling but Yocto SDK environment not sourced.\n"
    #         "Run: source environment-setup-* before CMake.")
    # endif()

    message(STATUS "Mode: Build for TARGET")
    
    pkg_check_modules(DRM REQUIRED libdrm)
    
else()
    message(STATUS "Mode: Build for HOST")

    find_package(SDL2 REQUIRED)

endif()

file(GLOB FONT_SOURCES CONFIGURE_DEPENDS "${FONTS_INPUT_DIR}/*.c")

message("FONT_SOURCES  ${FONT_SOURCES}")

add_library(${FONTS_LIB_NAME} STATIC ${FONT_SOURCES})

if(CMAKE_CROSSCOMPILING)
    target_compile_definitions(${FONTS_LIB_NAME} PRIVATE LV_CONF_SKIP=0 LV_CONF_PATH="${LV_CONFIG_PATH}" LV_USE_SDL=0)
    target_include_directories(${FONTS_LIB_NAME} PRIVATE ${LVGL_IMAGE_OUTPUT_DIR} ${LV_INCLUDE_PATH} ${DRM_INCLUDE_DIRS})
    target_link_libraries(${FONTS_LIB_NAME} ${DRM_LIBRARIES} m pthread)
else()
    target_compile_definitions(${FONTS_LIB_NAME} PRIVATE LV_CONF_SKIP=0 LV_CONF_PATH="${LV_CONFIG_PATH}" LV_USE_LINUX_DRM=0)
    target_include_directories(${FONTS_LIB_NAME} PRIVATE ${LVGL_IMAGE_OUTPUT_DIR} ${LV_INCLUDE_PATH})
    target_link_libraries(${FONTS_LIB_NAME} ${SDL2_LIBRARIES} m pthread)
endif()

# Install to prebuilt directory
add_custom_command(TARGET ${FONTS_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LVGL_RESOURCE_INSTALL_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${FONTS_LIB_NAME}> ${LVGL_RESOURCE_INSTALL_DIR}
    COMMENT "Installing lib${FONTS_LIB_NAME}.a to ${LVGL_RESOURCE_INSTALL_DIR}"
)
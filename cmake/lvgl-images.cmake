# @file CMakeLists.txt
# @author Alexandru ALEXANDRESCU
# All rights reserved.
#
# Image resources compilation for LVGL

cmake_minimum_required(VERSION 3.22)

if(NOT LVGL_RESOURCE_INSTALL_DIR)
    message(FATAL_ERROR "LVGL_RESOURCE_INSTALL_DIR not found !")
endif()

set(IMAGES_LIB_NAME images)

set(LVGL_IMAGE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/generated/lvgl_images)
set(LVGL_IMAGE_INPUT_DIR  ${CMAKE_SOURCE_DIR}/resources/images)
set(LV_CONFIG_PATH "${CMAKE_SOURCE_DIR}/external/lib_lvgl/lv_conf.h")
set(LV_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/lib_lvgl")
set(LV_SCRIPTS_PATH "${CMAKE_SOURCE_DIR}/external/lib_lvgl/lvgl/scripts")

if(STANDALONE_BUILD)
    set(LVGL_IMAGE_INPUT_DIR  ${CMAKE_SOURCE_DIR}/images)
    set(LV_CONFIG_PATH "${CMAKE_SOURCE_DIR}/../external/lib_lvgl/lv_conf.h")
    set(LV_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../external/lib_lvgl")
    set(LV_SCRIPTS_PATH "${CMAKE_SOURCE_DIR}/../external/lib_lvgl/lvgl/scripts")
endif()

set(PREFIX_NAME "img_")

find_package(PkgConfig REQUIRED)

# Python executable should be on HOST and not on the target !!!
if(CMAKE_CROSSCOMPILING)
    # if(NOT DEFINED ENV{SDKTARGETSYSROOT})
    #     message(FATAL_ERROR
    #         "Cross-compiling but Yocto SDK environment not sourced.\n"
    #         "Run: source environment-setup-* before CMake.")
    # endif()

    if(NOT DEFINED BUILD_PYTHON)
        message(FATAL_ERROR
            "Cross-compiling requires a HOST Python interpreter for LVGL image conversion.\n"
            "Please detect your host python BEFORE sourcing the SDK and pass it to CMake:\n"
            " Example:\n"
            "  export BUILD_PYTHON=$(which python3)\n"
            "  source environment-setup-*\n"
            "  cmake -DBUILD_PYTHON=${BUILD_PYTHON} <other args>\n"
        )
    endif()

    # Test that BUILD_PYTHON points to a usable python3
    if(NOT EXISTS "${BUILD_PYTHON}")
        message(FATAL_ERROR
            "BUILD_PYTHON = ${BUILD_PYTHON} does not exist.\n"
            "Please use a valid host python3 path."
        )
    endif()

    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.29.0")
        if(NOT IS_EXECUTABLE "${BUILD_PYTHON}")
            message(FATAL_ERROR
                "BUILD_PYTHON = ${BUILD_PYTHON} is not executable."
            )
        endif()
    endif()

    message(STATUS "Mode: Build for TARGET")
    
    pkg_check_modules(DRM REQUIRED libdrm)

    set(Python3_EXECUTABLE "${BUILD_PYTHON}")
    
else()
    message(STATUS "Mode: Build for HOST")

    find_package(SDL2 REQUIRED)

    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(Python3_EXECUTABLE ${Python3_EXECUTABLE})
endif()

# Output directory is recreated on every build
file(REMOVE_RECURSE "${LVGL_IMAGE_OUTPUT_DIR}")
file(MAKE_DIRECTORY "${LVGL_IMAGE_OUTPUT_DIR}")

file(GLOB_RECURSE LVGL_PNG_FILES
     LIST_DIRECTORIES false
     "${LVGL_IMAGE_INPUT_DIR}/*.png")

# Prepare the sources list
set(GENERATED_LVGL_IMAGE_SRCS)

foreach(img ${LVGL_PNG_FILES})

    get_filename_component(name ${img} NAME_WE)
    set(prefixed_name "${PREFIX_NAME}${name}")

    set(out "${LVGL_IMAGE_OUTPUT_DIR}/${prefixed_name}.c")

    add_custom_command(
        OUTPUT ${out}

        COMMAND ${Python3_EXECUTABLE} ${LV_SCRIPTS_PATH}/LVGLImage.py
                ${img}
                --output ${LVGL_IMAGE_OUTPUT_DIR}
                --ofmt C
                --cf ARGB8888
                --compress NONE
                --name ${prefixed_name}

        DEPENDS ${img}

        COMMENT "LVGL converting ${name}.png to ${prefixed_name}"
        VERBATIM
    )

    list(APPEND GENERATED_LVGL_IMAGE_SRCS ${out})

endforeach()

set_source_files_properties(
    ${GENERATED_LVGL_IMAGE_SRCS}
    PROPERTIES GENERATED TRUE
)

add_library(${IMAGES_LIB_NAME} STATIC
    ${GENERATED_LVGL_IMAGE_SRCS}
)

if(CMAKE_CROSSCOMPILING)
    target_compile_definitions(${IMAGES_LIB_NAME} PRIVATE LV_CONF_SKIP=0 LV_CONF_PATH="${LV_CONFIG_PATH}" LV_USE_SDL=0)
    target_include_directories(${IMAGES_LIB_NAME} PRIVATE ${LVGL_IMAGE_OUTPUT_DIR} ${LV_INCLUDE_PATH} ${DRM_INCLUDE_DIRS})
    target_link_libraries(${IMAGES_LIB_NAME} ${DRM_LIBRARIES} m pthread)
else()
    target_compile_definitions(${IMAGES_LIB_NAME} PRIVATE LV_CONF_SKIP=0 LV_CONF_PATH="${LV_CONFIG_PATH}" LV_USE_LINUX_DRM=0)
    target_include_directories(${IMAGES_LIB_NAME} PRIVATE ${LVGL_IMAGE_OUTPUT_DIR} ${LV_INCLUDE_PATH})
    target_link_libraries(${IMAGES_LIB_NAME} ${SDL2_LIBRARIES} m pthread)
endif()

# Install to prebuilt directory
add_custom_command(TARGET ${IMAGES_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LVGL_RESOURCE_INSTALL_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${IMAGES_LIB_NAME}> ${LVGL_RESOURCE_INSTALL_DIR}
    COMMENT "Installing lib${IMAGES_LIB_NAME}.a to ${LVGL_RESOURCE_INSTALL_DIR}"
)
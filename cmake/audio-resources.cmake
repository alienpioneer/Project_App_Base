# @file CMakeLists.txt
# @author Alexandru ALEXANDRESCU
# All rights reserved.
#
# Compile all wav sound files into a header file

cmake_minimum_required(VERSION 3.22)

set(AUDIO_LIB_NAME audiores)
set(AUDIO_HEADER_NAME audiores.hpp)

set(AUDIO_INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources/sounds")
set(AUDIO_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/lib_audiores")
set(SCRIPTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

set(AUDIO_HEADER "${CMAKE_CURRENT_BINARY_DIR}/generated/${AUDIO_HEADER_NAME}")

if(STANDALONE_BUILD)
    set(AUDIO_INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sounds")
    set(AUDIO_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../external/lib_audiores")
    set(SCRIPTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../scripts")
endif()

if(NOT TARGET_ELF_FORMAT)
    execute_process(
        COMMAND ${CMAKE_LINKER} --verbose
        OUTPUT_VARIABLE LD_VERBOSE_OUT
        ERROR_VARIABLE LD_VERBOSE_OUT
    )
    string(REGEX MATCH "OUTPUT_FORMAT\\(\"([^\"]+)\"" _ "${LD_VERBOSE_OUT}")
    set(TARGET_ELF_FORMAT ${CMAKE_MATCH_1})
endif()

message(STATUS "TARGET_ELF_FORMAT " ${TARGET_ELF_FORMAT})

# Python executable should be on HOST and not on the Yocto target !!!
if(CMAKE_CROSSCOMPILING)
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

    set(Python3_EXECUTABLE "${BUILD_PYTHON}")

    set(AUDIO_INSTALL_DIR ${AUDIO_OUTPUT_DIR}/target)
else()
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(Python3_EXECUTABLE ${Python3_EXECUTABLE})

    set(AUDIO_INSTALL_DIR ${AUDIO_OUTPUT_DIR}/host)
endif()

file(GLOB_RECURSE WAV_SOURCE_FILES
     LIST_DIRECTORIES false
     "${AUDIO_INPUT_DIR}/*.wav")

# Prepare the sources list
set(GENERATED_WAV_OBJECTS "")

foreach(WAV_IN ${WAV_SOURCE_FILES})
    get_filename_component(WAV_FILE_NAME ${WAV_IN} NAME)
    # Replaces '.' with '_' to match linker symbol naming
    string(REPLACE "." "_" SYMBOL_NAME ${WAV_FILE_NAME})
    set(GENERATED_WAV_OBJ "${CMAKE_CURRENT_BINARY_DIR}/${SYMBOL_NAME}.o")

    add_custom_command(
        OUTPUT ${GENERATED_WAV_OBJ}
        # COMMAND ld -r -b binary -o ${GENERATED_WAV_OBJ} ${WAV_FILE_NAME}
        COMMAND ${CMAKE_LINKER} -r -b binary --oformat ${TARGET_ELF_FORMAT} -o ${GENERATED_WAV_OBJ} ${WAV_FILE_NAME}
        DEPENDS ${WAV_IN}
        WORKING_DIRECTORY ${AUDIO_INPUT_DIR}
        VERBATIM
    )
    list(APPEND GENERATED_WAV_OBJECTS ${GENERATED_WAV_OBJ})
endforeach()

# Generate the audio registry header

add_custom_command(
    OUTPUT ${AUDIO_HEADER}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/generated"
    COMMAND ${Python3_EXECUTABLE} "${SCRIPTS_PATH}/compile_audio_resources.py" 
            "${AUDIO_INPUT_DIR}" "${AUDIO_HEADER}"
    DEPENDS ${WAV_SOURCE_FILES} "${SCRIPTS_PATH}/compile_audio_resources.py"
)

set_source_files_properties(${GENERATED_WAV_OBJECTS}
    PROPERTIES GENERATED TRUE
)

add_library(${AUDIO_LIB_NAME} STATIC ${GENERATED_WAV_OBJECTS} ${AUDIO_HEADER})
set_target_properties(${AUDIO_LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Install to prebuilt directory
add_custom_command(TARGET ${AUDIO_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${AUDIO_INSTALL_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${AUDIO_OUTPUT_DIR}/include

    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${AUDIO_LIB_NAME}> ${AUDIO_INSTALL_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${AUDIO_HEADER} ${AUDIO_OUTPUT_DIR}/include
    
    COMMENT "Installing lib${AUDIO_LIB_NAME}.a to ${AUDIO_INSTALL_DIR}"
)

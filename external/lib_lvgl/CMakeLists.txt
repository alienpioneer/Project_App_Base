cmake_minimum_required(VERSION 3.22)
project(lvgl_static_lib)

set(CMAKE_C_STANDARD 99)

set(TARGET_NAME lvgl)

if(NOT LVGL_INSTALL_DIR)
    set(LVGL_INSTALL_DIR ${CMAKE_SOURCE_DIR}/libs)
endif()

# ----------- OPTIONS

option(USE_GPU "Enable GPU rendering" ON)

# ----------- DEPENDENCIES

find_package(PkgConfig REQUIRED)

if(CMAKE_CROSSCOMPILING)

    message(STATUS "CMAKE_CROSSCOMPILING = ${CMAKE_CROSSCOMPILING}")

    pkg_check_modules(DRM REQUIRED libdrm)
    pkg_check_modules(GBM REQUIRED gbm)
    pkg_check_modules(EGL REQUIRED egl)
    pkg_check_modules(GLESV2 REQUIRED glesv2)
    
    set(LVGL_PREBUILT_DIR "${LVGL_INSTALL_DIR}/target")

else()

    find_package(SDL2 REQUIRED)
    set(LVGL_PREBUILT_DIR "${LVGL_INSTALL_DIR}/host")

endif()

# ----------- PATHS

set(LVGL_PREBUILT_LIB "${LVGL_PREBUILT_DIR}/liblvgl.a")
message(STATUS "Building LVGL static library ${LVGL_PREBUILT_LIB}...")

file(GLOB_RECURSE LVGL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/*.c)

if(NOT LVGL_SRC)
    message(FATAL_ERROR "LVGL sources not found")
endif()

add_library(${TARGET_NAME} STATIC ${LVGL_SRC})
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE C)

# ----------- Build settings

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
        LV_CONF_INCLUDE_SIMPLE
        LV_USE_SDL=0
        LV_USE_DRAW_SDL=0
        LV_USE_LINUX_DRM=1
        ${GPU_DEFINES}
    )
    target_include_directories(${TARGET_NAME} PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        lvgl
        ${DRM_INCLUDE_DIRS}
        ${GBM_INCLUDE_DIRS}
        ${EGL_INCLUDE_DIRS}
    )
    target_link_libraries(${TARGET_NAME} PUBLIC 
        ${DRM_LIBRARIES}
        ${GBM_LIBRARIES}
        ${EGL_LIBRARIES}
        ${GLESV2_LIBRARIES}
    )
else()

    set(SDL_DRAW_VALUE 0)

    if(USE_GPU)
        set(SDL_DRAW_VALUE 1)
    endif()

    target_compile_definitions(${TARGET_NAME} PRIVATE 
        LV_CONF_INCLUDE_SIMPLE
        LV_USE_SDL=1
        LV_USE_DRAW_SDL=${SDL_DRAW_VALUE}
        LV_USE_LINUX_DRM=0 
    )
    target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} lvgl ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${TARGET_NAME} PUBLIC ${SDL2_LIBRARIES})
endif()    
    
# Install to prebuilt directory
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LVGL_PREBUILT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> ${LVGL_PREBUILT_LIB}
    COMMENT "Installing liblvgl.a to prebuilt directory"
)

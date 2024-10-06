cmake_minimum_required(VERSION 3.20.0)
project(penumbra VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_library(penumbra STATIC
#     src/penumbra.cpp
# )

add_executable(penumbra
    src/utils/config.cpp
    src/penumbra.cpp
)

option(LINUX_X11 "Linux Only. Enables X11 support. [Recommended]" ON)
option(LINUX_WAYLAND "Linux Only. Enables Wayland support. [Recommended if Wayland is installed]" ON)

add_compile_definitions(BX_CONFIG_DEBUG=$<CONFIG:Debug>)

# Define PENUMBRA_X11_COMPAT and PENUMBRA_WL_COMPAT
if(LINUX)
    if(LINUX_X11)
        add_compile_definitions(PENUMBRA_X11_COMPAT)
    endif(LINUX_X11)
    if(LINUX_WAYLAND)
        add_compile_definitions(PENUMBRA_WL_COMPAT)
    endif(LINUX_WAYLAND)
endif(LINUX)


target_include_directories(${PROJECT_NAME} PRIVATE
    include
    src
)

target_link_libraries(${PROJECT_NAME}
    bgfx
    bimg
    bx
    glfw

    dl
    GL
    pthread
)

# If using X11, link it
if(LINUX)
    if(LINUX_X11)
        target_link_libraries(${PROJECT_NAME}
            X11
        )
    endif(LINUX_X11)
endif(LINUX)

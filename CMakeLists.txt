cmake_minimum_required(VERSION 3.20.0)

# Options
option(LINUX_X11 "Linux Only. Enables X11 support. [Recommended]" ON)
option(LINUX_WAYLAND "Linux Only. Enables Wayland support. [Recommended if Wayland is installed]" ON)

# INFO: To develop WGPU backend, disable building with BGFX and enable building with WGPU
option(BACKEND_BGFX "Use BGFX as Backend" ON)
option(BACKEND_WGPU "Use WebGPU Native as Backend" OFF)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# LIBRARY
project(penumbra VERSION 0.1.0 LANGUAGES C CXX)

add_library(${PROJECT_NAME} STATIC
    src/utils/vectors.cpp 
    src/utils/transform2D.cpp 

    src/antumbra/types/shader.cpp 
    src/antumbra/types/texture.cpp 
    src/antumbra/types/sprite.cpp 
    src/antumbra/antumbra.cpp 

    src/debug/consoleColors.cpp
    src/debug/log.cpp
    src/core/window.cpp
    src/utils/config.cpp
    src/penumbra.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE TRUE
)


add_compile_definitions(BX_CONFIG_DEBUG=$<CONFIG:Debug>)

# Add defines for backends
if(BACKEND_BGFX)
    add_compile_definitions(PENUMBRA_BACKEND_BGFX)
endif(BACKEND_BGFX)
if(BACKEND_WGPU)
    add_compile_definitions(PENUMBRA_BACKEND_WGPU)
endif(BACKEND_WGPU)


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

if(BACKEND_BGFX)
    target_link_libraries(${PROJECT_NAME}
        bgfx
        bimg
        bx
    )
endif(BACKEND_BGFX)

target_link_libraries(${PROJECT_NAME}
    glfw
    glm

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


# EXECUTABLE
project(penumbra-tests VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME}
    src/utils/vectors.cpp 
    src/utils/transform2D.cpp 

    src/antumbra/types/shader.cpp 
    src/antumbra/types/texture.cpp 
    src/antumbra/types/sprite.cpp 
    src/antumbra/antumbra.cpp 

    src/debug/consoleColors.cpp
    src/debug/log.cpp
    src/core/window.cpp
    src/utils/config.cpp
    src/penumbra.cpp

    testEntry.cpp
)

# Backend-specific sources
if(BACKEND_BGFX)
    target_sources(${PROJECT_NAME}
        PRIVATE src/backend/bgfx/backendWindow.cpp
        PRIVATE src/backend/bgfx/backendVtxLayout.cpp
        PRIVATE src/backend/bgfx/backendVtxBuffer.cpp
        PRIVATE src/backend/bgfx/backendIdxBuffer.cpp

        PRIVATE src/backend/bgfx/backendRender.cpp
    )
endif(BACKEND_BGFX)


add_compile_definitions(BX_CONFIG_DEBUG=$<CONFIG:Debug>)

# Add defines for backends
if(BACKEND_BGFX)
    add_compile_definitions(PENUMBRA_BACKEND_BGFX)
endif(BACKEND_BGFX)
if(BACKEND_WGPU)
    add_compile_definitions(PENUMBRA_BACKEND_WGPU)
endif(BACKEND_WGPU)

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

if(BACKEND_BGFX)
    target_link_libraries(${PROJECT_NAME}
        bgfx
        bimg
        bx
    )
endif(BACKEND_BGFX)

target_link_libraries(${PROJECT_NAME}
    glfw
    glm

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
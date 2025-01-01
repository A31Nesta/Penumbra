cmake_minimum_required(VERSION 3.20.0)

# Options
option(LINUX_X11 "Linux Only. Enables X11 support. [Recommended]" ON)
option(LINUX_WAYLAND "Linux Only. Enables Wayland support. [Recommended if Wayland is installed]" ON)

# INFO: To develop WGPU backend, disable building with BGFX and enable building with WGPU
option(BACKEND_BGFX "Use BGFX as Backend" OFF)
option(BACKEND_WGPU "Use WebGPU Native as Backend" ON)
# I never got Emscripten to work with CMake lol
# In Emscripten we should use the -sASYNCIFY option and the -sUSE_GLFW=3

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# LIBRARY
project(penumbra VERSION 0.1.0 LANGUAGES C CXX)

add_library(${PROJECT_NAME} STATIC
    src/utils/vectors.cpp 
    src/utils/transform2D.cpp 

    src/antumbra/types/sprite.cpp 
    src/antumbra/antumbra.cpp 

    src/debug/consoleColors.cpp
    src/debug/log.cpp
    src/core/window.cpp
    src/utils/config.cpp
    src/penumbra.cpp
)

# Backend-specific sources
if(BACKEND_BGFX)
    target_sources(${PROJECT_NAME} PRIVATE 
        src/backend/bgfx/backendRender.cpp
    
        src/backend/bgfx/shader.cpp 
        src/backend/bgfx/texture.cpp
    
        src/backend/bgfx/backendWindow.cpp
        src/backend/bgfx/backendVtxLayout.cpp
        src/backend/bgfx/backendVtxBuffer.cpp
        src/backend/bgfx/backendIdxBuffer.cpp
    )
endif(BACKEND_BGFX)

if(BACKEND_WGPU)
    target_sources(${PROJECT_NAME} PRIVATE 
        src/backend/wgpu/backendRender.cpp

        src/backend/wgpu/wgpuutils/objectManager.cpp
        src/backend/wgpu/wgpuutils/objManager/pipeline.cpp
        src/backend/wgpu/wgpuutils/init.cpp
        
        src/backend/wgpu/shader.cpp 
        src/backend/wgpu/texture.cpp
        
        src/backend/wgpu/backendWindow.cpp
        src/backend/wgpu/backendVtxLayout.cpp
        src/backend/wgpu/backendVtxBuffer.cpp
        src/backend/wgpu/backendIdxBuffer.cpp
    )
endif(BACKEND_WGPU)

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

# WARNING: Linux Only!
if(BACKEND_WGPU)
    target_include_directories(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/include>
        $<$<CONFIG:RelWithDebInfo>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/include>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/include>
        $<$<CONFIG:MinSizeRel>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/include>

        # Fix sussy issue where wgpu.h can't find the webgpu.h header
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/include/webgpu>
        $<$<CONFIG:RelWithDebInfo>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/include/webgpu>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/include/webgpu>
        $<$<CONFIG:MinSizeRel>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/include/webgpu>
    )
    target_link_directories(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/lib>
        $<$<CONFIG:RelWithDebInfo>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/lib>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/lib>
        $<$<CONFIG:MinSizeRel>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/lib>
    )
endif(BACKEND_WGPU)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(BACKEND_BGFX)
    target_link_libraries(${PROJECT_NAME}
        bgfx
        bimg
        bx
    )
endif(BACKEND_BGFX)

if(BACKEND_WGPU)
    target_link_libraries(${PROJECT_NAME}
        libwgpu_native.a
    )
endif(BACKEND_WGPU)

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
    testEntry.cpp
)

# Add compile warnings. 
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# Backend-specific sources
if(BACKEND_BGFX)
    target_sources(${PROJECT_NAME} PRIVATE 
        src/backend/bgfx/backendRender.cpp
    
        src/backend/bgfx/shader.cpp 
        src/backend/bgfx/texture.cpp
    
        src/backend/bgfx/backendWindow.cpp
        src/backend/bgfx/backendVtxLayout.cpp
        src/backend/bgfx/backendVtxBuffer.cpp
        src/backend/bgfx/backendIdxBuffer.cpp
    )
endif(BACKEND_BGFX)

if(BACKEND_WGPU)
    target_sources(${PROJECT_NAME} PRIVATE 
        src/backend/wgpu/backendRender.cpp

        src/backend/wgpu/wgpuutils/objectManager.cpp
        src/backend/wgpu/wgpuutils/objManager/pipeline.cpp
        src/backend/wgpu/wgpuutils/init.cpp
        
        src/backend/wgpu/shader.cpp 
        src/backend/wgpu/texture.cpp
        
        src/backend/wgpu/backendWindow.cpp
        src/backend/wgpu/backendVtxLayout.cpp
        src/backend/wgpu/backendVtxBuffer.cpp
        src/backend/wgpu/backendIdxBuffer.cpp
    )
endif(BACKEND_WGPU)

target_sources(${PROJECT_NAME} PRIVATE 
    src/utils/vectors.cpp 
    src/utils/transform2D.cpp 

    src/antumbra/types/sprite.cpp 
    src/antumbra/antumbra.cpp 

    src/debug/consoleColors.cpp
    src/debug/log.cpp
    src/core/window.cpp
    src/utils/config.cpp
    src/penumbra.cpp
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


# WARNING: Linux Only!
if(BACKEND_WGPU)
    target_include_directories(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/include>
        $<$<CONFIG:RelWithDebInfo>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/include>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/include>
        $<$<CONFIG:MinSizeRel>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/include>

        # Fix sussy issue where wgpu.h can't find the webgpu.h header
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/include/webgpu-headers>
        $<$<CONFIG:RelWithDebInfo>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/include/webgpu-headers>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/include/webgpu-headers>
        $<$<CONFIG:MinSizeRel>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/include/webgpu-headers>
    )
    target_link_directories(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/lib>
        $<$<CONFIG:RelWithDebInfo>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-debug/lib>
        $<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/lib>
        $<$<CONFIG:MinSizeRel>:${CMAKE_CURRENT_SOURCE_DIR}/external/wgpu-native/linux-release/lib>
    )
endif(BACKEND_WGPU)


target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(BACKEND_BGFX)
    target_link_libraries(${PROJECT_NAME}
        bgfx
        bimg
        bx
    )
endif(BACKEND_BGFX)

if(BACKEND_WGPU)
    target_link_libraries(${PROJECT_NAME}
        libwgpu_native.a
    )
endif(BACKEND_WGPU)

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
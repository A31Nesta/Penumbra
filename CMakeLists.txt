cmake_minimum_required(VERSION 3.20.0)
project(penumbra VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_library(penumbra STATIC
#     src/penumbra.cpp
# )

add_executable(penumbra
    src/penumbra.cpp
)

add_compile_definitions(BX_CONFIG_DEBUG=$<CONFIG:Debug>)

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
    X11
)
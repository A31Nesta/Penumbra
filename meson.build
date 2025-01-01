project('penumbra', 'c', 'cpp',
    default_options : {
        'cpp_std' : 'c++20',
        'prefer_static' : true,
        'warning_level' : 'everything',
        'b_staticpic' : true
    },
    version : '0.1.0'
)

# Get built-in options
debug = get_option('debug')
# Get custom options
pen_backend = get_option('pen_backend')
pen_linux_x11 = get_option('pen_linux_x11')
pen_linux_wayland = get_option('pen_linux_wayland')

# DEFINE ARGUMENTS
# ----------------
args = [
    '-DBX_CONFIG_DEBUG=' + (debug?'1':'0')
]
# Backend-specific files
if pen_backend == 'bgfx'
    args += '-DPENUMBRA_BACKEND_BGFX'
elif pen_backend == 'webgpu'
    args += '-DPENUMBRA_BACKEND_WGPU'
endif
# Linux-specific files
if host_machine.system() == 'linux'
    if pen_linux_x11
        args += '-DPENUMBRA_X11_COMPAT'
    endif
    if pen_linux_wayland
        args += '-DPENUMBRA_WL_COMPAT'
    endif
endif

# ADD INCLUDE DIRECTORIES
# -----------------------
includes = [
    include_directories(
        'include',
        'src'
    )
]

# DEFINE SOURCES
# --------------
# Backend-independent files
src = [
    # Test Entry, this is a library, the entry point is used for testing
    'testEntry.cpp',

    # Library sources
    'src/utils/vectors.cpp',
    'src/utils/transform2D.cpp',

    'src/antumbra/types/sprite.cpp',
    'src/antumbra/antumbra.cpp',

    'src/debug/consoleColors.cpp',
    'src/debug/log.cpp',
    'src/core/window.cpp',
    'src/utils/config.cpp',
    'src/penumbra.cpp'
]
# Rendering backend-specific files
if pen_backend == 'bgfx'
    src += [
        'src/backend/bgfx/backendRender.cpp',
    
        'src/backend/bgfx/shader.cpp',
        'src/backend/bgfx/texture.cpp',
    
        'src/backend/bgfx/backendWindow.cpp',
        'src/backend/bgfx/backendVtxLayout.cpp',
        'src/backend/bgfx/backendVtxBuffer.cpp',
        'src/backend/bgfx/backendIdxBuffer.cpp'
    ]
elif pen_backend == 'webgpu'
    src += [
        'src/backend/wgpu/backendRender.cpp',

        'src/backend/wgpu/wgpuutils/objectManager.cpp',
        'src/backend/wgpu/wgpuutils/objManager/pipeline.cpp',
        'src/backend/wgpu/wgpuutils/init.cpp',
        
        'src/backend/wgpu/shader.cpp',
        'src/backend/wgpu/texture.cpp',
        
        'src/backend/wgpu/backendWindow.cpp',
        'src/backend/wgpu/backendVtxLayout.cpp',
        'src/backend/wgpu/backendVtxBuffer.cpp',
        'src/backend/wgpu/backendIdxBuffer.cpp'
    ]
endif

# DEPENDENCY ISSUE
# ----------------
deps = []
# GLFW
deps += dependency('glfw3')
# GLM
deps += dependency('glm')
# WGPU
if pen_backend == 'webgpu'
    deps += dependency('wgpu-native')
    # Add wgpu-native-22.1.0.5/ffi/webgpu-headers Directory
    # This fixes "webgpu.h: No such file or directory" in wgpu.h
    includes += include_directories(
        'subprojects/wgpu-native-22.1.0.5/ffi/webgpu-headers'
    )
endif

executable('penumbra-tests', src, cpp_args : args, dependencies : deps, include_directories : includes)
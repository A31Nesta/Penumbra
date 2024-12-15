/lib/emscripten/emcc \
-lembind \
-lglfw \
-o build-emscripten/penumbra.html \
-I include/ \
-I src/ \
-I external/ \
-I external/wgpu-native/linux-release/include/webgpu \
-I external/wgpu-native/linux-release/include/ \
-std=c++20 \
-DPENUMBRA_BACKEND_WGPU \
-O2 \
testEntry.cpp \
\
src/backend/wgpu/backendRender.cpp \
src/backend/wgpu/wgpuutils/objManager/pipeline.cpp \
src/backend/wgpu/wgpuutils/objectManager.cpp \
src/backend/wgpu/wgpuutils/init.cpp \
src/backend/wgpu/shader.cpp  \
src/backend/wgpu/texture.cpp \
src/backend/wgpu/backendWindow.cpp \
src/backend/wgpu/backendVtxLayout.cpp \
src/backend/wgpu/backendVtxBuffer.cpp \
src/backend/wgpu/backendIdxBuffer.cpp \
\
src/utils/vectors.cpp  \
src/utils/transform2D.cpp  \
src/antumbra/types/sprite.cpp  \
src/antumbra/antumbra.cpp  \
src/debug/consoleColors.cpp \
src/debug/log.cpp \
src/core/window.cpp \
src/utils/config.cpp \
src/penumbra.cpp \
\
-s ENVIRONMENT="web" \
-s USE_WEBGPU \
-s USE_GLFW=3 \
-s ASYNCIFY \
-s ALLOW_MEMORY_GROWTH=1 \
--preload-file "./pnmbr"
# -s ASSERTIONS=1
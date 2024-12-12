#include "antumbra/types/sprite.hpp"

#ifdef PENUMBRA_BACKEND_WGPU
    #include "backend/wgpu/wgpuutils/objectManager.hpp"

#endif

namespace pen::antumbra {
    Sprite::Sprite(uint32_t id, Transform2D tr, Vec2 df, uint32_t tid, uint32_t sid) {
        this->id = id;
        transform = tr;
        transform.setDeform(df);
        textureID = tid;
        shaderID = sid;

        // Backend-specific data
        // WGPU
        // ----
        #ifdef PENUMBRA_BACKEND_WGPU
            // Create UniformData struct as pointer (we pass this as void*)
            backend::UniformData* uniformData = new backend::UniformData;

            // Create a binding

            // Convert to backendData
            backendData = reinterpret_cast<void*>(uniformData);
        #endif
    }

    Sprite::~Sprite() {
        #ifdef PENUMBRA_BACKEND_WGPU
            backend::UniformData* uniformData = reinterpret_cast<backend::UniformData*>(backendData);
            // Delete WGPU Objects
            // wgpuBindGroupRelease(uniformData->bindGroup);
            // wgpuBufferRelease(uniformData->uniformBuffer);
            // Delete this pointer
            delete uniformData;
        #endif
    }

    void Sprite::setDeform(Vec2 deform) {
        transform.setDeform(deform);
    }
}
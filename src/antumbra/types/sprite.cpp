#include "antumbra/types/sprite.hpp"
#include <glm/fwd.hpp>

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

            // Create buffer
            // TODO: MOVE SOMEWHERE ELSE
            WGPUBufferDescriptor uniformBufferDesc{};
            uniformBufferDesc.nextInChain = nullptr;
            uniformBufferDesc.size = sizeof(glm::mat4);
            uniformBufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
            uniformBufferDesc.mappedAtCreation = false;
            uniformData->uniformBuffer = wgpuDeviceCreateBuffer(backend::objects::device, &uniformBufferDesc);

            // Set a default Model
            glm::mat4 model;
            wgpuQueueWriteBuffer(backend::objects::queue, uniformData->uniformBuffer, 0, &model, sizeof(glm::mat4));

            WGPUBindGroupEntry binding{};
            binding.nextInChain = nullptr;
            // Index of the binding. Model has the index 0 of Group 1
            binding.binding = 0;
            // It is pointing to the uniform buffer in the viewProjection object
            binding.buffer = uniformData->uniformBuffer;
            // Offset
            binding.offset = 0;
            // Size
            binding.size = sizeof(glm::mat4);

            // Create Bind Group Descriptor
            WGPUBindGroupDescriptor bindGroupDesc{};
            bindGroupDesc.nextInChain = nullptr;
            bindGroupDesc.layout = backend::pipeline2D::bindGroupLayouts[1];
            bindGroupDesc.entryCount = 1;
            bindGroupDesc.entries = &binding;
            bindGroupDesc.label = "modelMatrix";
            uniformData->bindGroup = wgpuDeviceCreateBindGroup(backend::objects::device, &bindGroupDesc);

            // Convert to backendData
            backendData = reinterpret_cast<void*>(uniformData);
        #endif
    }

    Sprite::~Sprite() {
        #ifdef PENUMBRA_BACKEND_WGPU
            backend::UniformData* uniformData = reinterpret_cast<backend::UniformData*>(backendData);
            // Delete WGPU Objects
            wgpuBindGroupRelease(uniformData->bindGroup);
            wgpuBufferRelease(uniformData->uniformBuffer);
            // Delete this pointer
            delete uniformData;
        #endif
    }

    void Sprite::setDeform(Vec2 deform) {
        transform.setDeform(deform);
    }
}
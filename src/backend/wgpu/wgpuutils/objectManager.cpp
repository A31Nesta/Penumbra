#include "objectManager.hpp"

#include "backend/wgpu/wgpuutils/objManager/pipeline.hpp"
#include <webgpu-headers/webgpu.h>

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

#include <iostream>

namespace pen::backend {
    // Global Data
    namespace objects {
        WGPUDevice device;
        WGPUQueue queue;
        // Surface
        WGPUSurface surface;
        WGPUSurfaceCapabilities surfaceCapabilities;
        WGPUSurfaceConfiguration config;
        // Pipeline
        WGPUTextureFormat surfaceFormat = WGPUTextureFormat_Undefined;

        // View and Projection Uniforms (general)
        UniformData viewProjection;
    }
    namespace layouts {
        // We specify that this layout is used for 2D Only
        WGPUVertexBufferLayout* layout2D;
    }

    // Pipeline Templates and Bind Group Layouts
    namespace pipeline2D {
        // 0 - View and Perspective
        // 1 - Model
        // 2 - Texture
        WGPUBindGroupLayout bindGroupLayouts[3];
        WGPUPipelineLayout pipelineLayout;
    }
    
    // WGPU Pipeline register
    std::vector<WGPURenderPipeline*> renderPipelines;
    std::vector<WGPUBuffer*> vertexBuffers;
    std::vector<WGPUBuffer*> indexBuffers;
    std::vector<TextureData*> textures;
    std::vector<UniformData*> uniforms;

    // Inits the Bind Group Layouts, duh
    void initBindGroupLayouts() {
        create2DBindGroupLayouts(
            pipeline2D::bindGroupLayouts[0],
            pipeline2D::bindGroupLayouts[1],
            pipeline2D::bindGroupLayouts[2],
            objects::device
        );

        WGPUPipelineLayoutDescriptor layoutDesc{};
        layoutDesc.nextInChain = nullptr;
        layoutDesc.bindGroupLayoutCount = 3; // View and Projection + Model + Texture
        layoutDesc.bindGroupLayouts = pipeline2D::bindGroupLayouts;
        pipeline2D::pipelineLayout = wgpuDeviceCreatePipelineLayout(objects::device, &layoutDesc);
    }
    // Inits the Bind Group for the View and Projection
    void initViewProjectionUniform() {
        // Create buffer
        WGPUBufferDescriptor uniformBufferDesc{};
        uniformBufferDesc.nextInChain = nullptr;
        uniformBufferDesc.size = sizeof(ViewProjection);
        uniformBufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
        uniformBufferDesc.mappedAtCreation = false;
        objects::viewProjection.uniformBuffer = wgpuDeviceCreateBuffer(objects::device, &uniformBufferDesc);

        // Set a default View and Projection at first
        ViewProjection vp;
        wgpuQueueWriteBuffer(objects::queue, objects::viewProjection.uniformBuffer, 0, &vp, sizeof(ViewProjection));

        WGPUBindGroupEntry binding{};
        binding.nextInChain = nullptr;
        // Index of the binding. View and Projection has the index 0
        binding.binding = 0;
        // It is pointing to the uniform buffer in the viewProjection object
        binding.buffer = objects::viewProjection.uniformBuffer;
        // Offset
        binding.offset = 0;
        // Size
        binding.size = sizeof(ViewProjection);

        // Create Bind Group Descriptor
        WGPUBindGroupDescriptor bindGroupDesc{};
        bindGroupDesc.nextInChain = nullptr;
        bindGroupDesc.layout = pipeline2D::bindGroupLayouts[0];
        bindGroupDesc.entryCount = 1;
        bindGroupDesc.entries = &binding;
        bindGroupDesc.label = "viewProjectionMatrices";
        objects::viewProjection.bindGroup = wgpuDeviceCreateBindGroup(objects::device, &bindGroupDesc);
    }

    // Registers a pipeline and returns its ID
    uint16_t registerPipeline(WGPURenderPipeline* pipeline) {
        // Check for re-usable spots
        for (uint16_t i = 0; i < renderPipelines.size(); i++) {
            WGPURenderPipeline* p = renderPipelines.at(i);
            if (p == nullptr) {
                renderPipelines.at(i) = pipeline;
                return i;
            }
        }

        // If we didn't have free spots,
        // we should just add this new one
        uint16_t index = renderPipelines.size();
        renderPipelines.push_back(pipeline);
        return index;
    }
    // Registers a vertex buffer and returns its ID
    uint16_t registerVertexBuffer(WGPUBuffer* buffer) {
        // Check for re-usable spots
        for (uint16_t i = 0; i < vertexBuffers.size(); i++) {
            WGPUBuffer* buf = vertexBuffers.at(i);
            if (buf == nullptr) {
                vertexBuffers.at(i) = buffer;
                return i;
            }
        }
        // If we didn't have free spots,
        // we should just add this new one
        uint16_t index = vertexBuffers.size();
        vertexBuffers.push_back(buffer);
        return index;
    }
    // Registers an index buffer and returns its ID
    uint16_t registerIndexBuffer(WGPUBuffer* buffer) {
        // Check for re-usable spots
        for (uint16_t i = 0; i < indexBuffers.size(); i++) {
            WGPUBuffer* buf = indexBuffers.at(i);
            if (buf == nullptr) {
                indexBuffers.at(i) = buffer;
                return i;
            }
        }
        // If we didn't have free spots,
        // we should just add this new one
        uint16_t index = indexBuffers.size();
        indexBuffers.push_back(buffer);
        return index;
    }
    // Registers a texture and returns its ID
    uint16_t registerTexture(TextureData* texture) {
        // Check for re-usable spots
        for (uint16_t i = 0; i < textures.size(); i++) {
            TextureData* tex = textures.at(i);
            if (tex == nullptr) {
                textures.at(i) = texture;
                return i;
            }
        }

        // If we didn't have free spots,
        // we should just add this new one
        uint16_t index = textures.size();
        textures.push_back(texture);
        return index;
    }
    // Registers a uniform and returns its ID
    uint16_t registeruniform(UniformData* uniform) {
        // Check for re-usable spots
        for (uint16_t i = 0; i < uniforms.size(); i++) {
            UniformData* unf = uniforms.at(i);
            if (unf == nullptr) {
                uniforms.at(i) = uniform;
                return i;
            }
        }

        // If we didn't have free spots,
        // we should just add this new one
        uint16_t index = uniforms.size();
        uniforms.push_back(uniform);
        return index;
    }

    // Takes a path to a shader and returns the ID of the pipeline
    uint16_t createRenderPipeline(std::string shaderCode) {
        // RENDER PIPELINE
        WGPURenderPipeline* pipeline = new WGPURenderPipeline;

        // Create Shader module
        WGPUShaderModuleDescriptor shaderDesc = {};
        #ifndef __EMSCRIPTEN__
            shaderDesc.hintCount = 0;
            shaderDesc.hints = nullptr;
        #endif

        // Shader Code Module Descriptor
        WGPUShaderModuleWGSLDescriptor shaderCodeDesc = {}; // We use WGSL for shaders. We could use SPIR-V or GLSL though
        shaderCodeDesc.chain.next = nullptr;
        shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        // Connect Chain
        shaderDesc.nextInChain = &shaderCodeDesc.chain;
        // Set Shader Source
        shaderCodeDesc.code = shaderCode.c_str();

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(objects::device, &shaderDesc);

        // Init render pipeline
        // We set it to default 2D Parameters
        WGPURenderPipelineDescriptor pipelineDesc = create2DPipelineDescriptor(layouts::layout2D, shaderModule, objects::surfaceFormat);
        
        // Set Layout here.
        pipelineDesc.layout = pipeline2D::pipelineLayout;

        // Create render pipeline
        *pipeline = wgpuDeviceCreateRenderPipeline(objects::device, &pipelineDesc);

        // Delete shader module. 1 pipeline per shader so....
        wgpuShaderModuleRelease(shaderModule);

        // REGISTER PIPELINE AND RETURN ID
        // -------------------------------
        return registerPipeline(pipeline);
    }
    // Sets the pipeline specified
    void setRenderPipeline(WGPURenderPassEncoder renderPass, uint16_t pipeline) {
        wgpuRenderPassEncoderSetPipeline(renderPass, *renderPipelines.at(pipeline));
    }
    // Destroy render pipeline
    void destroyRenderPipeline(uint16_t pipeline) {
        wgpuRenderPipelineRelease(*renderPipelines.at(pipeline));
        delete renderPipelines.at(pipeline);
        renderPipelines.at(pipeline) = nullptr;
    }

    // Creates Vertex Buffer from a vector of Vertices
    uint16_t createVertexBuffer(std::vector<float> vtxArr) {
        WGPUBuffer* vtxBuffer = new WGPUBuffer;

        // Descriptor
        WGPUBufferDescriptor bufferDescriptor{};
        bufferDescriptor.nextInChain = nullptr;
        bufferDescriptor.size = vtxArr.size() * sizeof(float);
        bufferDescriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex; // Copy from CPU to GPU + It's a Vertex Buffer
        bufferDescriptor.mappedAtCreation = false;
        
        *vtxBuffer = wgpuDeviceCreateBuffer(objects::device, &bufferDescriptor);

        // Send buffer to GPU Memory
        wgpuQueueWriteBuffer(objects::queue, *vtxBuffer, 0, vtxArr.data(), bufferDescriptor.size);

        // REGISTER VERTEX BUFFER AND RETURN ID
        // ------------------------------------
        return registerVertexBuffer(vtxBuffer);
    }
    // Destroy Vertex Buffer
    void destroyVertexBuffer(uint16_t buffer) {
        // I prefer having full control over when the object is released
        // With buffers I can call Destroy instead of Release to destroy the object now
        wgpuBufferDestroy(*vertexBuffers.at(buffer));
        wgpuBufferRelease(*vertexBuffers.at(buffer));
        delete vertexBuffers.at(buffer);
        vertexBuffers.at(buffer) = nullptr;
    }

    // Creates Index Buffer from a vector of Indices
    uint16_t createIndexBuffer(std::vector<uint16_t> idxArr) {
        WGPUBuffer* idxBuffer = new WGPUBuffer;

        // Descriptor
        WGPUBufferDescriptor bufferDescriptor{};
        bufferDescriptor.nextInChain = nullptr;
        bufferDescriptor.size = idxArr.size() * sizeof(uint16_t);
        bufferDescriptor.size = (bufferDescriptor.size + 3) & ~3; // round up to the next multiple of 4
        bufferDescriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index; // Copy from CPU to GPU + It's a Index Buffer
        bufferDescriptor.mappedAtCreation = false;
        
        *idxBuffer = wgpuDeviceCreateBuffer(objects::device, &bufferDescriptor);

        // Write buffer to GPU Memory
        wgpuQueueWriteBuffer(objects::queue, *idxBuffer, 0, idxArr.data(), bufferDescriptor.size);

        // REGISTER INDEX BUFFER AND RETURN ID
        // ------------------------------------
        return registerIndexBuffer(idxBuffer);
    }
    // Destroy Index Buffer
    void destroyIndexBuffer(uint16_t buffer) {
        // I prefer having full control over when the object is released
        // With buffers I can call Destroy instead of Release to destroy the object now
        wgpuBufferDestroy(*indexBuffers.at(buffer));
        wgpuBufferRelease(*indexBuffers.at(buffer));
        delete indexBuffers.at(buffer);
        indexBuffers.at(buffer) = nullptr;
    }

    // Creates a Texture and Texture View
    uint16_t createTexture(std::vector<uint8_t> data, uint16_t w, uint16_t h) {
        // Get Mip Level Count. We always generate all of 'em for funsies
        uint32_t maxMipLevelCount = std::bit_width(std::max(w, h));

        // CREATE TEXTURE DATA STRUCT
        // --------------------------
        TextureData* textureData = new TextureData();

        // Create Texture
        // --------------
        WGPUTextureDescriptor textureDesc;
        // Size Stuff
        textureDesc.dimension = WGPUTextureDimension_2D;
        textureDesc.size = {w, h, 1};
        textureDesc.mipLevelCount = maxMipLevelCount; // We paid for all mipmaps so we're using all mipmaps
        textureDesc.sampleCount = 1;
        // Format, we assume RGBA8 for no reason in particular
        // INFO: Assuming RGBA8 for no reason in particular could give issues lol
        textureDesc.format = WGPUTextureFormat_RGBA8Unorm;
        // Usage and view format stuff (whatever that is)
        textureDesc.usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding;
        textureDesc.viewFormatCount = 0;
        textureDesc.viewFormats = nullptr;
        // Create Texture
        textureData->texture = wgpuDeviceCreateTexture(objects::device, &textureDesc);
        std::cout << "PENUMBRA_INFO [WGPU]: Created WGPUTexture!\n";

        // Create the View
        // ---------------
        WGPUTextureViewDescriptor textureViewDesc;
        textureViewDesc.aspect = WGPUTextureAspect_All;
        textureViewDesc.baseArrayLayer = 0;
        textureViewDesc.arrayLayerCount = 1;
        textureViewDesc.baseMipLevel = 0;
        textureViewDesc.mipLevelCount = maxMipLevelCount;
        textureViewDesc.dimension = WGPUTextureViewDimension_2D;
        textureViewDesc.format = textureDesc.format;
        textureData->textureView = wgpuTextureCreateView(textureData->texture, &textureViewDesc);
        std::cout << "PENUMBRA_INFO [WGPU]: Created Texture View!\n";

        // Create a sampler
        WGPUSamplerDescriptor samplerDesc;
        samplerDesc.addressModeU = WGPUAddressMode_Repeat;
        samplerDesc.addressModeV = WGPUAddressMode_Repeat;
        samplerDesc.addressModeW = WGPUAddressMode_Repeat;
        samplerDesc.magFilter = WGPUFilterMode_Linear;
        samplerDesc.minFilter = WGPUFilterMode_Linear;
        samplerDesc.mipmapFilter = WGPUMipmapFilterMode_Linear;
        samplerDesc.lodMinClamp = 0.0f;
        samplerDesc.lodMaxClamp = maxMipLevelCount; // All mipmaps because funi
        samplerDesc.compare = WGPUCompareFunction_Undefined;
        samplerDesc.maxAnisotropy = 1;
        textureData->sampler = wgpuDeviceCreateSampler(objects::device, &samplerDesc);
        std::cout << "PENUMBRA_INFO [WGPU]: Created Sampler!\n";

        // Write to Texture
        // ----------------
        // Arguments telling which part of the texture we upload to
        // (together with the last argument of writeTexture)
        WGPUImageCopyTexture destination;
        destination.texture = textureData->texture;
        destination.origin = { 0, 0, 0 }; // equivalent of the offset argument of wgpuQueueWriteBuffer
        destination.aspect = WGPUTextureAspect_All; // only relevant for depth/Stencil textures

        // Arguments telling how the C++ side pixel memory is laid out
        WGPUTextureDataLayout source;
        source.offset = 0;

        WGPUExtent3D mipLevelSize = textureDesc.size;
        uint32_t previousLevelWidth = 0;
        std::vector<uint8_t> previousLevelPixels;
        for (uint32_t level = 0; level < textureDesc.mipLevelCount; ++level) {
            // Create pixels array
            std::vector<uint8_t> pixels(4 * mipLevelSize.width * mipLevelSize.height);
            // Create image data for this mip level
            // If level is 0 we copy
            if (level == 0) {
                pixels = data;
            }
            // If the level is more than 0 (a mipmap)
            else {
                for (uint32_t i = 0; i < mipLevelSize.width; ++i) {
                    for (uint32_t j = 0; j < mipLevelSize.height; ++j) {
                        uint8_t* p = &pixels[4 * (j * mipLevelSize.width + i)];
                        
                        // Get the corresponding 4 pixels from the previous level
                        uint8_t* p00 = &previousLevelPixels[4 * ((2 * j + 0) * (previousLevelWidth) + (2 * i + 0))];
                        uint8_t* p01 = &previousLevelPixels[4 * ((2 * j + 0) * (previousLevelWidth) + (2 * i + 1))];
                        uint8_t* p10 = &previousLevelPixels[4 * ((2 * j + 1) * (previousLevelWidth) + (2 * i + 0))];
                        uint8_t* p11 = &previousLevelPixels[4 * ((2 * j + 1) * (previousLevelWidth) + (2 * i + 1))];

                        // Average
                        p[0] = (p00[0] + p01[0] + p10[0] + p11[0]) / 4;
                        p[1] = (p00[1] + p01[1] + p10[1] + p11[1]) / 4;
                        p[2] = (p00[2] + p01[2] + p10[2] + p11[2]) / 4;
                        p[3] = (p00[3] + p01[3] + p10[3] + p11[3]) / 4;
                    }
                }
            }

            // Change this to the current level
            destination.mipLevel = level;

            // Compute from the mip level size
            source.bytesPerRow = 4 * mipLevelSize.width;
            source.rowsPerImage = mipLevelSize.height;

            wgpuQueueWriteTexture(objects::queue, &destination, pixels.data(), pixels.size(), &source, &mipLevelSize);

            // Before getting the next texture size we store the current
            previousLevelWidth = mipLevelSize.width;

            // The size of the next mip level:
            // (see https://www.w3.org/TR/webgpu/#logical-miplevel-specific-texture-extent)
            // Modified to disallow size 0 images
            mipLevelSize.width  = std::max((uint32_t)1, mipLevelSize.width  / 2);
            mipLevelSize.height = std::max((uint32_t)1, mipLevelSize.height / 2);

            previousLevelPixels = std::move(pixels);
        }
        std::cout << "PENUMBRA_INFO [WGPU]: Moved Texture Data to GPU!\n";

        // Create the Bind Group
        // ---------------------
        // Texture
        WGPUBindGroupEntry bindGroupEntries[2];
        bindGroupEntries[0].nextInChain = nullptr;
        bindGroupEntries[0].binding = 0; // new bind group so...
        bindGroupEntries[0].textureView = textureData->textureView;
        bindGroupEntries[0].buffer = nullptr;
        bindGroupEntries[0].sampler = nullptr;
        // Sampler
        bindGroupEntries[1].nextInChain = nullptr;
        bindGroupEntries[1].binding = 1; // Sampler has binding 1
        bindGroupEntries[1].sampler = textureData->sampler;
        bindGroupEntries[1].textureView = nullptr;
        bindGroupEntries[1].buffer = nullptr;
        
        WGPUBindGroupDescriptor bindGroupDesc{};
        bindGroupDesc.nextInChain = nullptr;
        bindGroupDesc.layout = pipeline2D::bindGroupLayouts[2]; // Index 2 is for the texture
        bindGroupDesc.entryCount = 2;
        bindGroupDesc.entries = bindGroupEntries;
        bindGroupDesc.label = "textureBindGroup";
        textureData->textureBindGroup = wgpuDeviceCreateBindGroup(objects::device, &bindGroupDesc);
        std::cout << "PENUMBRA_INFO [WGPU]: Created Bind Groups!\n";

        // REGISTER TEXTURE AND RETURN ID
        // ------------------------------
        return registerTexture(textureData);
    }
    // Destroys the texture and texture view
    void destroyTexture(uint16_t texture) {
        TextureData* td = textures.at(texture);
        // Delete Bind Group
        wgpuBindGroupRelease(td->textureBindGroup);
        // Delete Sampler
        wgpuSamplerRelease(td->sampler);
        // Delete texture
        wgpuTextureViewRelease(td->textureView);
        wgpuTextureDestroy(td->texture);
        wgpuTextureRelease(td->texture);

        // Delete texture Data
        delete td;
        textures.at(texture) = nullptr;
    }


    // UNIFORMS
    // Creates a UniformData object (bind group and buffer)
    uint16_t createUniformData(uint64_t bufferSize, uint16_t bindingLayoutID, std::string label) {
        UniformData* uniformData = new UniformData;

        // Create buffer
        WGPUBufferDescriptor uniformBufferDesc{};
        uniformBufferDesc.nextInChain = nullptr;
        uniformBufferDesc.size = bufferSize; // sizeof(glm::mat4) for Model Matrix, for example
        uniformBufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
        uniformBufferDesc.mappedAtCreation = false;
        uniformData->uniformBuffer = wgpuDeviceCreateBuffer(objects::device, &uniformBufferDesc);

        // Set a default Model
        // We don't do this here because this function is general-purpose, should be fine though
        // glm::mat4 model;
        // wgpuQueueWriteBuffer(objects::queue, uniformData->uniformBuffer, 0, &model, sizeof(glm::mat4));

        WGPUBindGroupEntry binding{};
        binding.nextInChain = nullptr;
        // Index of the binding. This is a new Bind Group, we use Binding 0. For multiple data you use Structs and Binding 0
        binding.binding = 0;
        // It is pointing to the uniform buffer created earlier
        binding.buffer = uniformData->uniformBuffer;
        // Offset
        binding.offset = 0;
        // Size
        binding.size = bufferSize;

        // Create Bind Group Descriptor
        WGPUBindGroupDescriptor bindGroupDesc{};
        bindGroupDesc.nextInChain = nullptr;
        bindGroupDesc.layout = backend::pipeline2D::bindGroupLayouts[bindingLayoutID]; // bindingLayoutID is 1 for Model Matrices
        bindGroupDesc.entryCount = 1;
        bindGroupDesc.entries = &binding;
        bindGroupDesc.label = label.c_str(); // "modelMatrix", for example. Can be whatever
        uniformData->bindGroup = wgpuDeviceCreateBindGroup(backend::objects::device, &bindGroupDesc);

        // Set Group Index
        uniformData->groupIndex = bindingLayoutID;

        // REGISTER UNIFORM AND RETURN ID
        // ------------------------------
        return registeruniform(uniformData);
    }
    // Destroys the Uniform Data
    void destroyUniformData(uint16_t uniformData) {
        UniformData* ud = uniforms.at(uniformData);

        // Delete WGPU Objects
        wgpuBindGroupRelease(ud->bindGroup);
        wgpuBufferRelease(ud->uniformBuffer);
        // Delete this pointer
        delete ud;
        uniforms.at(uniformData) = nullptr;
    }


    // Sets the Vertex and Index Buffers
    void setBuffers(WGPURenderPassEncoder& renderPass, uint16_t vertexBuffer, uint16_t indexBuffer) {
        WGPUBuffer vtxBuf = *vertexBuffers.at(vertexBuffer);
        WGPUBuffer idxBuf = *indexBuffers.at(indexBuffer);

        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vtxBuf, 0, wgpuBufferGetSize(vtxBuf));
        wgpuRenderPassEncoderSetIndexBuffer(renderPass, idxBuf, WGPUIndexFormat_Uint16, 0, wgpuBufferGetSize(idxBuf));
    }
    // Sets a Bind Group and writes to buffer
    void setBindGroup(WGPURenderPassEncoder& renderPass, uint16_t uniform, void* data, uint64_t sizeofData) {
        UniformData* uniformData = uniforms.at(uniform);
        wgpuRenderPassEncoderSetBindGroup(renderPass, uniformData->groupIndex, uniformData->bindGroup, 0, nullptr);
        wgpuQueueWriteBuffer(objects::queue, uniformData->uniformBuffer, 0, data, sizeofData);
    }
    // Binds a texture
    void bindTexture(WGPURenderPassEncoder& renderPass, uint16_t texture) {
        TextureData textureData = *textures.at(texture);
        // Sets the Texture Bind Group into the Group Index 2 (Texture)
        wgpuRenderPassEncoderSetBindGroup(renderPass, 2, textureData.textureBindGroup, 0, nullptr);
    }


    // Deinitializes all objects
    void deinitAllObjects() {
        // Release pipeline layout
        wgpuPipelineLayoutRelease(pipeline2D::pipelineLayout);
        // Bind Group release (for View and Projection matrices)
        wgpuBindGroupRelease(objects::viewProjection.bindGroup);
        // Release Bind Group Layout
        wgpuBindGroupLayoutRelease(pipeline2D::bindGroupLayouts[0]);
        wgpuBindGroupLayoutRelease(pipeline2D::bindGroupLayouts[1]);
        // Delet uniform buffer
        wgpuBufferDestroy(objects::viewProjection.uniformBuffer);
        wgpuBufferRelease(objects::viewProjection.uniformBuffer);

        // Unconfigure the surface
        wgpuSurfaceUnconfigure(objects::surface);
        // Free members
        wgpuSurfaceCapabilitiesFreeMembers(objects::surfaceCapabilities);
        // Release
        wgpuQueueRelease(objects::queue);
        wgpuSurfaceRelease(objects::surface);
        wgpuDeviceRelease(objects::device);
    }
}
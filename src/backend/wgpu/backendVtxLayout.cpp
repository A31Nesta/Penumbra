#include "../backendVtxLayout.hpp"
#include "webgpu.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace pen::backend {
    std::unordered_map<BackendAttribute, uint32_t> penToWGPUShaderLocation {
        {Position, 0},  // Position
        {TexCoord0, 1}, // UV Mapping 1
        {TexCoord1, 2}, // UV Mapping 2 for lights
        {Normal, 3},    // After UVs we have Normals, Tangents and Bitangents for Normal Maps
        {Tangent, 4},
        {Bitangent, 5},
        // Stuff we probably won't use
        {Color0, -1},
        {Color1, -1},
        {Color2, -1},
        {Color3, -1},
        {Indices, -1},
        {Weight, -1},
        {TexCoord2, -1},
        {TexCoord3, -1},
        {TexCoord4, -1},
        {TexCoord5, -1},
        {TexCoord6, -1},
        {TexCoord7, -1}
    };

    // Destructor. If data has something, delete it
    BackendVtxLayout::~BackendVtxLayout() {
        // Only delete if we have data and we haven't copied
        // the data pointer
        if (!hasCopied && data != nullptr) {
            // Delete vertex layout data...
        }
    }

    // Adds a Vertex Attribute to the Array
    void BackendVtxLayout::addVtxAttrib(BackendAttribute vtxAttrib) {
        vtxLayout.push_back(vtxAttrib);
    }

    // Get the backend-specific data. This data is created by the
    // source file inside this function. This is only done once
    void* BackendVtxLayout::getBackendSpecificData() {
        // If we already calculated the data we return it
        if (data) {
            return data;
        }

        // If we didn't calculate it before, do it now
        WGPUVertexBufferLayout vertexBufferLayout{};
        std::vector<WGPUVertexAttribute> attributes;
        uint16_t offset = 0;

        // Loop
        for (BackendAttribute attr : vtxLayout) {
            WGPUVertexFormat format = WGPUVertexFormat_Float32x3; // Defaults to 3 Floats, changes to 2 floats for UVs
            if (attr == BackendAttribute::TexCoord0 || attr == BackendAttribute::TexCoord1) {
                format = WGPUVertexFormat_Float32x2;
            }

            // Create attribute and add it to the list
            WGPUVertexAttribute vtxAttribute;
            vtxAttribute.shaderLocation = penToWGPUShaderLocation[attr]; // Set Shader Location
            vtxAttribute.format = format; // Set Format (2 or 3 Floats)
            vtxAttribute.offset = offset; // Set Offset. For the first attribute it is 0

            // Add to attribute list
            attributes.push_back(vtxAttribute);

            // Update Offset
            offset += (format==WGPUVertexFormat_Float32x3)?(sizeof(float) * 3):(sizeof(float) * 2);
        }
        
        // 

        // Save data pointer and return
        // data = (void*) <Whatever the fuck goes here>;
        return data;
    }

    // Get size of data array
    uint16_t BackendVtxLayout::_getDataSize() {
        // return sizeof whatever data is
        return 0;
    }
}
#include "../backendVtxLayout.hpp"
#include <bgfx/bgfx.h>
#include <unordered_map>

namespace pen::backend {
    std::unordered_map<BackendAttribute, bgfx::Attrib::Enum> penToBGFX {
        {Position, bgfx::Attrib::Position},
        {Normal, bgfx::Attrib::Normal},
        {Tangent, bgfx::Attrib::Tangent},
        {Bitangent, bgfx::Attrib::Bitangent},
        {Color0, bgfx::Attrib::Color0},
        {Color1, bgfx::Attrib::Color1},
        {Color2, bgfx::Attrib::Color2},
        {Color3, bgfx::Attrib::Color3},
        {Indices, bgfx::Attrib::Indices},
        {Weight, bgfx::Attrib::Weight},
        {TexCoord0, bgfx::Attrib::TexCoord0},
        {TexCoord1, bgfx::Attrib::TexCoord1},
        {TexCoord2, bgfx::Attrib::TexCoord2},
        {TexCoord3, bgfx::Attrib::TexCoord3},
        {TexCoord4, bgfx::Attrib::TexCoord4},
        {TexCoord5, bgfx::Attrib::TexCoord5},
        {TexCoord6, bgfx::Attrib::TexCoord6},
        {TexCoord7, bgfx::Attrib::TexCoord7}
    };

    // Maps BackendAttribute to number of floats necessary
    // INFO: We use floats only because the "indices" attribute is probably unnecessary idk
    std::unordered_map<BackendAttribute, uint8_t> attribToNum {
        {Position,  3},
        {Normal,    3},
        {Tangent,   3},
        {Bitangent, 3},
        {Color0,    3},
        {Color1,    3},
        {Color2,    3},
        {Color3,    3},
        {Indices,   1}, // What the hell is this?
        {Weight,    1},
        {TexCoord0, 2},
        {TexCoord1, 2},
        {TexCoord2, 2},
        {TexCoord3, 2},
        {TexCoord4, 2},
        {TexCoord5, 2},
        {TexCoord6, 2},
        {TexCoord7, 2}
    };

    // Destructor. If data has something, delete it
    BackendVtxLayout::~BackendVtxLayout() {
        // Only delete if we have data and we haven't copied
        // the data pointer
        if (!hasCopied && data != nullptr) {
            bgfx::VertexLayout* bgfxVtxLayout = (bgfx::VertexLayout*)data;
            delete bgfxVtxLayout;
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
        bgfx::VertexLayout* bgfxVtxLayout = new bgfx::VertexLayout;
        bgfxVtxLayout->begin();
        // Get data from array
        for (BackendAttribute attr : vtxLayout) {
            bgfx::Attrib::Enum bgfxAttr = penToBGFX[attr];
            uint8_t numOfFloats = attribToNum[attr];

            bgfxVtxLayout->add(bgfxAttr, numOfFloats, bgfx::AttribType::Float);
        }
        // End definition
        bgfxVtxLayout->end();

        // Save data pointer and return
        data = (void*)bgfxVtxLayout;
        return data;
    }

    // Get size of data array
    uint16_t BackendVtxLayout::_getDataSize() {
        return sizeof(bgfx::VertexLayout);
    }
}
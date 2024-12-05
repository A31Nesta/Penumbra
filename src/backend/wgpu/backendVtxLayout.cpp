#include "../backendVtxLayout.hpp"

#include <unordered_map>

namespace pen::backend {
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
        // Create the Vertex Layout

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
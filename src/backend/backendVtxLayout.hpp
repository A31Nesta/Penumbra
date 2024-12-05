#pragma once

/**
    Backend-independent Vertex Layout
    ---------------------------------

    Basically a Vertex Array Object from OpenGL.
    

*/

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace pen::backend {
    // ENUMS: Mostly the same as in BGFX
    // ---------------------------------

    // Possible Attributes:
    enum BackendAttribute {
        Position,  //!< a_position      | 3 Floats
        Normal,    //!< a_normal        | 3 Floats
        Tangent,   //!< a_tangent       | 3 Floats
        Bitangent, //!< a_bitangent     | 3 Floats
        Color0,    //!< a_color0        | 3 Floats (No Alpha for you :[)
        Color1,    //!< a_color1        | 3 Floats
        Color2,    //!< a_color2        | 3 Floats
        Color3,    //!< a_color3        | 3 Floats
        Indices,   //!< a_indices       | Idk wtf is this lol
        Weight,    //!< a_weight        | 1 Float (probably?)
        TexCoord0, //!< a_texcoord0     | 2 Floats
        TexCoord1, //!< a_texcoord1     | 2 Floats
        TexCoord2, //!< a_texcoord2     | 2 Floats
        TexCoord3, //!< a_texcoord3     | 2 Floats
        TexCoord4, //!< a_texcoord4     | 2 Floats
        TexCoord5, //!< a_texcoord5     | 2 Floats
        TexCoord6, //!< a_texcoord6     | 2 Floats
        TexCoord7, //!< a_texcoord7     | 2 Floats
    };

    // CLASS: Creates a backend-specific VAO from backend-independent
    // data.
    class BackendVtxLayout {
    public:
        BackendVtxLayout() {}
        BackendVtxLayout(BackendVtxLayout& otherBVL) {
            if (otherBVL.hasCopied) {
                throw std::runtime_error("PENUMBRA_ERROR: Tried to copy the same BackendVtxLayout twice");
            }

            // Copy the vector
            vtxLayout = otherBVL.vtxLayout;
            // Copy the pointer. We want it to point to the same place
            data = otherBVL.data;
            
            // If we copied the object we won't delete it here
            otherBVL.hasCopied = true;
        }
        ~BackendVtxLayout();

        // Adds a Vertex Attribute to the Array
        void addVtxAttrib(BackendAttribute vtxAttrib);

        // Get the backend-specific data. This data is created by the
        // source file inside this function. This is only done once
        void* getBackendSpecificData();


        // Functions for Copy Constructor
        // ------------------------------

        // Get size of data array (backend-specific)
        static uint16_t _getDataSize();
        // Returns true if the data pointer is not null pointer
        bool _hasData() { return data != nullptr; }

    private:
        // The backend-specific data. It is only calculated the first
        // time we call getBackendSpecificData();
        void* data;

        // All the attributes in this object
        std::vector<BackendAttribute> vtxLayout;

        // Indicates if it has copied the data to another
        // object
        bool hasCopied = false;
    };
}
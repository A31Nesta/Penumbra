#include "../backendRender.hpp"

namespace pen::backend {
    // Global variables
    uint32_t framebuffer = 0; // In this case it is the view ID

    // FUNCTIONS
    // ---------

    // Shading and Uniforms
    uint16_t createUniform(std::string name, UniformType type) {
        // Implement here...
        return 0;
    }
    void deleteUniform(uint16_t uniform) {
        // Delete uniform...
    }

    // Framebuffer Stuff
    void bindFramebuffer(uint32_t framebufferID) {
        framebuffer = framebufferID;

        // Do some stuff here. Assigning to a "framebuffer" variable may not even
        // be necessary
    }

    // General
    void setViewTransform(float* viewMtx, float* projMtx) {
        // Set View and Projection matrices
    }

    // Models
    void setModelTransform(float* modelMtx) {
        // Set Model Transform matrix
    }
    void setBuffers(BackendVtxBuffer* bvb, BackendIdxBuffer* bib) {
        // Bind the Vertex Buffer and Index Buffer
    }

    // Draw Element
    void drawCurrent(Shader* shader) {
        // Some code to draw the currently bound element...
    }
}
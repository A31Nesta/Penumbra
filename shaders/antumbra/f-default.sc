$input a_texcoord0

#include <bgfx_shader.sh>

void main() {
    gl_FragColor = vec4(a_texcoord0.x, a_texcoord0.y, 0, 1);
}
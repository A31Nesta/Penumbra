$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_color, 0);

void main() {
    gl_FragColor = vec4(vec3(v_texcoord0.x, v_texcoord0.y, 1), texture2D(s_color, v_texcoord0).a);
}
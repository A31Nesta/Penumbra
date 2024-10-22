$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_color, 0);

void main() {
    vec4 color = texture2D(s_color, v_texcoord0);

    gl_FragColor = color.rgba;
}
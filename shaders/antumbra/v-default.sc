$input a_position, a_texcoord0
$output v_texcoord0

#include <bgfx_shader.sh>

void main() {
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    v_texcoord0.x = a_texcoord0.x;
    v_texcoord0.y = 1.0 - a_texcoord0.y;
}
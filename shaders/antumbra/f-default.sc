$input v_texcoord0

#include <bgfx_shader.sh>

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float distToCenter = distance(v_texcoord0, vec2(0.5, 0.5));
    if (distToCenter > 0.5) {
        discard;
    }

    gl_FragColor = vec4(hsv2rgb(vec3(distToCenter*2, 1, 1)), 1.0);
}
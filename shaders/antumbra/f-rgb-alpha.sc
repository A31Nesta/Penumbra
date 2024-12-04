$input v_texcoord0

#include <bgfx_shader.sh>

#define M_PI 3.1415926535897932384626433832795

SAMPLER2D(s_color, 0);

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float distToCenter = distance(v_texcoord0, vec2(0.5, 0.5));
    vec2 toCenter = vec2_splat(0.5)-v_texcoord0;
    float angle = atan2(toCenter.y, toCenter.x);
    float angleMapped = angle/(2.0*M_PI);

    gl_FragColor = vec4(hsv2rgb(vec3(angleMapped, distToCenter*2, 1)), texture2D(s_color, v_texcoord0).a);
}
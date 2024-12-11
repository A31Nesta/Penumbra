struct VertexInput {
    @location(0) position: vec3f,
    @location(1) uv: vec2f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
	// The location here does not refer to a vertex attribute, it just means
	// that this field must be handled by the rasterizer.
	// (It can also refer to another field of another struct that would be used
	// as input to the fragment shader.)
	@location(0) uv: vec2f,
};

// VERTEX CODE
@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.position = vec4f(in.position, 1.0);
    output.uv = in.uv;
    return output;
}

// FRAGMENT CODE
fn hsv2rgb(c: vec3f) -> vec3f {
    let K: vec4f = vec4f(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    let p: vec3f = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, vec3f(0.0), vec3f(1.0)), c.y);
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let PI = 3.14159274101257324;
    
    let distToCenter: f32 = distance(in.uv, vec2(0.5, 0.5));
    let toCenter: vec2f = vec2f(0.5)-in.uv;
    let angle: f32 = atan2(toCenter.y, toCenter.x);
    let angleMapped: f32 = angle/(2.0*PI);

    // Discard fragments that are too far :D
    if (distToCenter > 0.5) {
        discard;
    }

    // Gamma-correction
    let linear_color: vec3f = pow(hsv2rgb(vec3f(angleMapped, distToCenter*2, 1)), vec3f(2.2));
    return vec4f(linear_color, 1.0);
}
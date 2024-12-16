// DEFAULT UNIFORM STRUCT
struct ViewProjection {
    view: mat4x4<f32>,
    projection: mat4x4<f32>,
}

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

// Uniforms
@group(0) @binding(0) var<uniform> uViewProj: ViewProjection;
@group(1) @binding(0) var<uniform> uModel: mat4x4<f32>;

// Textures
@group(2) @binding(0) var uTexture: texture_2d<f32>;
@group(2) @binding(1) var uSampler: sampler;

// VERTEX CODE
@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.position = uViewProj.projection * uViewProj.view * uModel * vec4f(in.position, 1.0);
    output.uv = in.uv;
    return output;
}

// FRAGMENT CODE
@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    // Gamma-correction
    let linear_color: vec3f = pow(vec3f(in.uv.x, in.uv.y, 1), vec3f(2.2));
    let alpha: f32 = textureSample(uTexture, uSampler, in.uv).a;
    return vec4f(linear_color*alpha, alpha);
}

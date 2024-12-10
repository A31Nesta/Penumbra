@vertex
fn vs_main(@location(0) in_vertex_pox: vec3f) -> @builtin(position) vec4f {
    return vec4f(in_vertex_pox, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    return vec4f(1.0, 1.0, 1.0, 1.0);
}
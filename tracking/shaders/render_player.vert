#version 150

uniform mat4 modelViewProjectionMatrix;

#define BODY_INDEX_MAP_BACKGROUND 255

uniform sampler2DRect depth_texture;
uniform sampler2DRect body_index_texture;
uniform sampler2DRect world_texture;

uniform ivec2 frame_size;
uniform int[6] body_ids;

uniform int player_id;

out vec4 vColor;

void main() {
    vec2 tex_coord = vec2(gl_InstanceID % frame_size.x, gl_InstanceID / frame_size.x);

    float depth = texture(depth_texture, tex_coord).x;
    int body_index = int(texture(body_index_texture, tex_coord).x * 255);
    vec4 ray = texture(world_texture, tex_coord);

    if (depth != 0 && body_index != BODY_INDEX_MAP_BACKGROUND && body_ids[body_index] == player_id && ray.x != 0 && ray.y != 0) {
        vColor = vec4(0.0, 1.0, 0, 1.0);
    } else {
        vColor = vec4(0.0);
    }

    vec4 posWorld = vec4(1);
    posWorld.z = depth * 65535.0;
    posWorld.x = ray.x * posWorld.z;
    posWorld.y = ray.y * posWorld.z;

    gl_Position = modelViewProjectionMatrix * posWorld;
}

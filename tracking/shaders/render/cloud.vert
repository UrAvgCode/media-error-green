#version 150

uniform mat4 modelViewProjectionMatrix;

#define BODY_INDEX_MAP_BACKGROUND 255

uniform sampler2DRect depth_texture;
uniform sampler2DRect body_index_texture;
uniform sampler2DRect world_texture;

uniform ivec2 frame_size;
uniform int[6] body_ids;

uniform int player_id;

uniform int time;

out vec4 vColor;

void main() {
    vec2 tex_coord = vec2(gl_InstanceID % frame_size.x, gl_InstanceID / frame_size.x);

    float depth = texture(depth_texture, tex_coord).x;
    int body_index = int(texture(body_index_texture, tex_coord).x * 255);
    vec4 ray = texture(world_texture, tex_coord);

    if (body_ids[body_index] == player_id && depth != 0 && ray.x != 0 && ray.y != 0) {
        vColor = vec4(0, 1, 0, 1);
    } else {
        vColor = vec4(0);
    }

    vec4 posWorld = vec4(1);
    posWorld.z = depth * 65535.0;
    posWorld.x = ray.x * posWorld.z;
    posWorld.y = ray.y * posWorld.z;

    float min_grid_size = 15;
    float max_grid_size = 20;
    float speed = 0.003;

    float amplitude = (max_grid_size - min_grid_size) / 2.0;
    float offset = (max_grid_size + min_grid_size) / 2.0;
    float grid_size = amplitude * sin(time * speed) + offset;

    posWorld.x = floor(posWorld.x / grid_size) * grid_size;
    posWorld.y = floor(posWorld.y / grid_size) * grid_size;
    posWorld.z = floor(posWorld.z / grid_size) * grid_size;

    gl_Position = modelViewProjectionMatrix * posWorld;
}

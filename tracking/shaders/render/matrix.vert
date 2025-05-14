#version 150

uniform mat4 modelViewProjectionMatrix;

#define BODY_INDEX_MAP_BACKGROUND 255

uniform sampler2DRect depth_texture;
uniform sampler2DRect body_index_texture;
uniform sampler2DRect world_texture;

// Matrix effect texture is a Rect texture
uniform sampler2DRect matrix_texture;

uniform ivec2 frame_size;
uniform int[6] body_ids;
uniform int player_id;
uniform int time;

out vec4 vColor;

void main() {
    vec2 matrix_texture_size = textureSize(matrix_texture);

    vec2 tex_coord = vec2(gl_InstanceID % frame_size.x, gl_InstanceID / frame_size.x);

    float depth = texture(depth_texture, tex_coord).x;
    int body_index = int(texture(body_index_texture, tex_coord).x * 255);
    vec4 ray = texture(world_texture, tex_coord);

    vec4 pos_world = vec4(1);
    pos_world.z = depth * 65535.0;
    pos_world.x = ray.x * pos_world.z;
    pos_world.y = ray.y * pos_world.z;
    pos_world.w = 1.0;

    gl_Position = modelViewProjectionMatrix * pos_world;

    if (body_ids[body_index] == player_id && depth != 0 && ray.x != 0 && ray.y != 0) {
        const float scale = 0.4;
        const float speed = 0.1;

        vec2 matrix_texcoord;
        matrix_texcoord.x = mod(pos_world.x * scale, matrix_texture_size.x);
        matrix_texcoord.y = mod(pos_world.y * scale + time * speed, matrix_texture_size.y);

        vColor = texture(matrix_texture, matrix_texcoord);
    } else {
        vColor = vec4(0.0);
    }
}
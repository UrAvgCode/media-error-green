#version 150

// OF built-in attributes.
uniform mat4 modelViewProjectionMatrix;

// Custom attributes.
#define BODY_INDEX_MAP_BACKGROUND 255

// Textures
uniform sampler2DRect depth_texture;
uniform sampler2DRect body_index_texture;
uniform sampler2DRect world_texture;

uniform ivec2 frame_size;
uniform int[6] body_ids;

// Glitch Effect
uniform float time;
uniform float random_offset_one;
uniform float random_offset_two;

// Screen Shake
uniform float screen_shake_amplitude;

out vec4 vColor;

void main()
{
    vec2 tex_coord = vec2(gl_InstanceID % frame_size.x, gl_InstanceID / frame_size.x);

    float depth = texture(depth_texture, tex_coord).x;
    int body_index = int(texture(body_index_texture, tex_coord).x * 255);
    vec4 ray = texture(world_texture, tex_coord);

    if (depth != 0 && body_index != BODY_INDEX_MAP_BACKGROUND && ray.x != 0 && ray.y != 0) {
        int body_id = body_ids[body_index];
        vColor = vec4(0.0, 255.0, 0.0, 1.0);
    } else {
        vColor = vec4(0.0);
    }

    vec4 posWorld = vec4(1);
    posWorld.z = depth * 65535.0;
    posWorld.x = ray.x * posWorld.z;
    posWorld.y = ray.y * posWorld.z;

    // glitch effect
    const float glitch_width = 40;
    const float glitch_height = 10;

    if (posWorld.y < random_offset_one + glitch_height && posWorld.y > random_offset_one - glitch_height) {
        posWorld.x += glitch_width;
    }

    if (posWorld.y < random_offset_two + glitch_height && posWorld.y > random_offset_two - glitch_height) {
        posWorld.x -= glitch_width;
    }

    // Screen Shake
    const float screen_shake_offset_x = sin(time * 5.0) * screen_shake_amplitude;
    const float screen_shake_offset_y = cos(time * 5.0) * screen_shake_amplitude;

    posWorld.x += screen_shake_offset_x;
    posWorld.y += screen_shake_offset_y;

    gl_Position = modelViewProjectionMatrix * posWorld;
}


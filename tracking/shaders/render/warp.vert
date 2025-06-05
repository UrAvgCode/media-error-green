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

out vec4 v_color;
out vec3 v_normal;
out vec3 v_frag_pos;

vec3 normal_vector(vec2 tex_coord, vec4 pos_world) {
    vec3 normal = vec3(0.0, 0.0, 1.0);
    if (tex_coord.x < frame_size.x - 1 && tex_coord.y < frame_size.y - 1) {
        // get positions of adjacent pixels
        vec2 right_coord = tex_coord + vec2(1.0, 0.0);
        vec2 bottom_coord = tex_coord + vec2(0.0, 1.0);
        
        float right_depth = texture(depth_texture, right_coord).x;
        float bottomDepth = texture(depth_texture, bottom_coord).x;
        
        vec4 right_ray = texture(world_texture, right_coord);
        vec4 bottom_ray = texture(world_texture, bottom_coord);
        
        vec3 right_pos = vec3(
            right_ray.x * right_depth * 65535.0,
            right_ray.y * right_depth * 65535.0,
            right_depth * 65535.0
        );
        
        vec3 bottom_pos = vec3(
            bottom_ray.x * bottomDepth * 65535.0,
            bottom_ray.y * bottomDepth * 65535.0,
            bottomDepth * 65535.0
        );
        
        vec3 currentPos = vec3(pos_world.x, pos_world.y, pos_world.z);
        
        // calculate normal from adjacent positions
        if (right_depth > 0.0 && bottomDepth > 0.0) {
            vec3 right_vec = right_pos - currentPos;
            vec3 bottom_vec = bottom_pos - currentPos;
            normal = normalize(cross(right_vec, bottom_vec));
        }
    }

    return normal;
}

void main() {
    vec2 tex_coord = vec2(gl_InstanceID % frame_size.x, gl_InstanceID / frame_size.x);

    float depth = texture(depth_texture, tex_coord).x;
    int body_index = int(texture(body_index_texture, tex_coord).x * 255);
    vec4 ray = texture(world_texture, tex_coord);

    vec4 pos_world = vec4(1);
    pos_world.z = depth * 65535.0;
    pos_world.x = ray.x * pos_world.z;
    pos_world.y = ray.y * pos_world.z;
    
    v_normal = normal_vector(tex_coord, pos_world);
    v_frag_pos = vec3(pos_world.xyz);

    const float amplitude = 20.0;
    pos_world.x += sin(pos_world.y * 0.05 + time / 100.0) * amplitude;

    if (body_ids[body_index] == player_id && depth != 0 && ray.x != 0 && ray.y != 0) {
        v_color = vec4(0.0, 1.0, 0.0, 1.0);
    } else {
        v_color = vec4(0.0);
    }
    
    gl_Position = modelViewProjectionMatrix * pos_world;
}

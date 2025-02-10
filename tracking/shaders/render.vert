#version 150

// OF built-in attributes.
uniform mat4 modelViewProjectionMatrix;

// Custom attributes.
#define BODY_INDEX_MAP_BACKGROUND 255

const vec4[6] COLORS = vec4[]
(
    vec4(211 / 255.0, 248 / 255.0, 226 / 255.0, 1.0),
    vec4(228 / 255.0, 193 / 255.0, 249 / 255.0, 1.0),
    vec4(237 / 255.0, 231 / 255.0, 177 / 255.0, 1.0),
    vec4(246 / 255.0, 148 / 255.0, 193 / 255.0, 1.0),
    vec4(169 / 255.0, 222 / 255.0, 249 / 255.0, 1.0),
    vec4(255 / 255.0, 135 / 255.0, 111 / 255.0, 1.0)
);

uniform sampler2DRect uDepthTex;
uniform sampler2DRect uBodyIndexTex;
uniform sampler2DRect uWorldTex;

uniform ivec2 uFrameSize;
uniform int[6] uBodyIDs;

uniform float time;
uniform float random_offset_one;
uniform float random_offset_two;

uniform float shake_amplitudes[6]; // ARRAY FÜR JEDEN BODY
uniform float screen_shake_amplitude; // Neue Uniform für Screen Shake

out vec4 vColor;

void main()
{
    vec2 texCoord = vec2(gl_InstanceID % uFrameSize.x, gl_InstanceID / uFrameSize.x);

    float depth = texture(uDepthTex, texCoord).x;
    int bodyIndex = int(texture(uBodyIndexTex, texCoord).x * 255);
    vec4 ray = texture(uWorldTex, texCoord);

    if (depth != 0 && 
        bodyIndex != BODY_INDEX_MAP_BACKGROUND && 
        ray.x != 0 && ray.y != 0)
    {
        int bodyID = uBodyIDs[bodyIndex];
        vColor = vec4(0.0, 255.0, 0.0, 1.0);
    }
    else
    {
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

    posWorld.x += sin(posWorld.y + time) * 20.0;

    // Individual Body Shake
    // float shake_strength = shake_amplitudes[bodyIndex % 6]; 
    // posWorld.x += sin(time * 10.0 + posWorld.y) * shake_strength;
    // posWorld.y += cos(time * 10.0 + posWorld.x) * shake_strength;

    // Screen Shake
    float screen_shake_offset_x = sin(time * 5.0) * screen_shake_amplitude;
    float screen_shake_offset_y = cos(time * 5.0) * screen_shake_amplitude;

    posWorld.x += screen_shake_offset_x;
    posWorld.y += screen_shake_offset_y;

    gl_Position = modelViewProjectionMatrix * posWorld;
}


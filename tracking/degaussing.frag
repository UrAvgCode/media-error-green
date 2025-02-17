#version 150

uniform sampler2DRect tex0;
uniform float intensity;
uniform float distortionAmount;
uniform float time;
uniform vec2 resolution;

in vec2 vTexCoord;
out vec4 fragColor;

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

float lines(in vec2 pos, float b){
    float scale = 10.0;
    pos *= scale;
    return smoothstep(0.0,
                    .5+b*.5,
                    abs((sin(pos.x*3.1415)+b*2.0))*.5);
}

void main() {

    float wavePattern1 = sin(vTexCoord.x * 0.15 + time) * distortionAmount * intensity;
    float wavePattern2 = (sin(vTexCoord.x * 0.14 - time +2) * 5.0);

    vec2 rotatedCoord = rotate2d(1.0) * vTexCoord;
    float rotatedPattern2 = (sin(rotatedCoord.y * 0.14 - time +2) * 5.0);

    float pct = 0.0;

    pct = distance(vTexCoord/resolution,vec2(0.5));

    float moireePattern = wavePattern1 + wavePattern2;

    float distortedY = vTexCoord.y + moireePattern * distortionAmount * 5.0;


    vec4 color = texture(tex0, vTexCoord);

    color.g += (intensity) * 0.1 * (sin((distortedY * 0.05) + 0.0) * 0.5 + 0.5);
    color.r += (intensity) * 0.1 * (sin((distortedY * 0.05) + 4.18) * 0.5 + 0.5);
    color.b += (intensity) * 0.1 * (sin((distortedY * 0.05) + 2.09) * 0.5 + 0.5);

    color.rgb = floor(color.rgb * (256.0)) / (256.0);
    //color.a = color.a * pct;

    fragColor = color;
}
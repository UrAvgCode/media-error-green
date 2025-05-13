#version 150

uniform sampler2DRect tex0;
uniform float time;
uniform float block_size;

in vec2 vTexCoord;
out vec4 fragColor;

float rand(vec2 n) { 
	return fract(sin(dot(n + time, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	
	float res = mix(
		mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
	return res*res;
}

void main() {
	vec2 block_coord = floor(vTexCoord / block_size) * block_size;

    float gray_value = noise(block_coord);
    float alpha = texture(tex0, block_coord).a;

    fragColor = vec4(0, gray_value, 0, alpha);
}
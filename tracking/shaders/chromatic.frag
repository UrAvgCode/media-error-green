#version 150

uniform sampler2DRect tex0;
uniform float aberration_amount;
uniform float time;
uniform float rand1;
uniform float rand2;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec2 offset = vec2(aberration_amount, 0.0);

    vec2 new_tex_coord = vec2(vTexCoord.x, vTexCoord.y);
    //new_tex_coord.x += sin(new_tex_coord.y + time) * 10.0f;  // Verzerrt die einzelnen Zeilen des Renderings wellenförmig.

    //if (vTexCoord.y < rand1 + 5 && vTexCoord.y > rand1 - 5) {  //Verschiebt zufällige Zeilenblöcke nach rechts.
    //    new_tex_coord.x += 30.0f;
    //}

    //if (vTexCoord.y < rand2 + 5 && vTexCoord.y > rand2 - 5) {  //Verschiebt zufällige Zeilenblöcke nach links.
    //    new_tex_coord.x -= 30.0f;
    //}

    vec3 color;
    color.r = texture(tex0, new_tex_coord + offset   // Arrbaration - Verschieben von Rot nach rechts.
    ).r;
    color.g = texture(tex0, new_tex_coord).g;
    color.b = texture(tex0, new_tex_coord - offset   // Arrbaration - Verschieben von Blau nach links.
    ).b;

    float alpha = 1.0;

    fragColor = vec4(color, alpha);
}
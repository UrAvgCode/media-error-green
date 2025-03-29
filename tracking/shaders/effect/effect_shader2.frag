#version 150

uniform sampler2DRect tex0;
uniform float block_size;
uniform float quality;

in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    // Texturfarbe an der aktuellen Koordinate abfragen
    vec4 color = texture(tex0, vTexCoord);

    // Überprüfen, ob der Punkt sichtbar ist (z.B. nicht transparent)
    if (color.a > 0.0) {
        // Alle sichtbaren Punkte rot färben
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Rot mit voller Deckkraft
    } else {
        // Transparente Punkte unverändert lassen
        fragColor = color;
    }
}
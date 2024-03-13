#version 330

// Input
	in vec3 frag_position;
    in vec3 frag_normal;
    in vec2 frag_texture;
    in vec3 frag_color;

// Output
layout(location = 0) out vec4 out_color;

// Uniforms
uniform sampler2D texture;
uniform int isGround;
uniform int isTank;
uniform int isPlayer;
uniform int health;

void main()
{
	vec3 frag_color_modif = frag_color;

	// if fragment is from enemy tank, display health using colors: modify color slowly to red
	if(isTank == 1 && isPlayer == 0) {
		if (health == 3) {
			// no hits taken
		} else if (health == 2) {
			// 1 hit taken
			frag_color_modif = vec3(frag_color_modif.r + 0.1, frag_color_modif.g - 0.002, frag_color_modif.b - 0.002);
		} else if (health == 1) {
			// 2 hits taken
			frag_color_modif = vec3(frag_color_modif.r + 0.2, frag_color_modif.g - 0.004, frag_color_modif.b - 0.004);
		} else {
			// 3 hits taken
			frag_color_modif = vec3(frag_color_modif.r + 0.3, frag_color_modif.g - 0.006, frag_color_modif.b - 0.006);
		}
	}

    // set output color or texture
	if(isGround == 0) {
		out_color = vec4(frag_color_modif, 1.0);
	} else {
		out_color = texture2D(texture, frag_texture);
	}
}
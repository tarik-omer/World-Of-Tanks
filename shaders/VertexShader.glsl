#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform int isTank;
uniform int isPlayer;
uniform int health;

uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// Output
out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture;
out vec3 frag_color;

void main()
{
	vec3 v_position_modif = v_position;

    // if vertex is from the player's tank, based on health modify position of vertex to create a damaged look
	if (isTank == 1)
	{
		if (health == 3)
		{
			// took no damage - no change to position
		} else if (health == 2) {
			// took damage once
			v_position_modif.y = v_position.y - 0.1;
			v_position_modif.x = v_position.x - 0.17;
			v_position_modif.z = v_position.z - 0.25;
		} else if (health == 1) {
			// took damage twice
			v_position_modif.y = v_position.y - 0.2;
			v_position_modif.x = v_position.x - 0.34;
			v_position_modif.z = v_position.z - 0.5;
		} else if (health <= 0) {
			// took damage three times and is dead
			v_position_modif.y = v_position.y - 0.3;
			v_position_modif.x = v_position.x - 0.9;
			v_position_modif.z = v_position.z - 0.75;
		}
	}

	// Compute world space vectors
	vec3 world_position = (Model * vec4(v_position_modif, 1)).xyz;
	vec3 world_normal	= normalize(mat3(Model) * normalize(v_normal));

	vec3 V = normalize(eye_position - world_position);
	vec3 L = normalize(light_position - world_position);
	vec3 H = normalize(L + V);
	vec3 R = normalize(reflect(L, world_normal));

	// Define ambient light component
	float ambient_light = 0.75;

	// Compute diffuse light component
	float diffuse_light = material_kd * max(dot(world_normal, L), 0);

	// Compute specular light component
	float specular_light = 0.5;

	if (diffuse_light > 0)
	{
		specular_light = material_ks * pow(max(dot(V, R), 0), material_shininess);
	}

	// Compute light
	float d	= distance(light_position, v_position_modif);
	float attenuation_factor = 1 / (1 + 0.14 * d + 0.07 * d * d);
	float light	= ambient_light + attenuation_factor * (diffuse_light + specular_light);

	frag_position = v_position_modif;
    frag_normal = v_normal;
    frag_texture = v_texture;
	frag_color = v_color * light;

    gl_Position = Projection * View * Model * vec4(v_position_modif, 1.0);
}
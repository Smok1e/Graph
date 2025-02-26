#version 430 core

//===========================================

uniform vec4  background_color;
uniform vec4  background_dot_color;
uniform float background_dot_radius;
uniform float background_dot_distance;
uniform vec2  center;
uniform bool  enable;

out vec4 out_Color;

//===========================================

void main()
{
	if (enable)
	{
		vec2 position = mod(
			gl_FragCoord.xy + center * vec2(1, -1) + background_dot_distance / 2, 
			background_dot_distance
		) - background_dot_distance / 2;

		out_Color = length(position) <= background_dot_radius
			? background_dot_color
			: background_color;
	}

	else
		out_Color = background_color;
}

//===========================================
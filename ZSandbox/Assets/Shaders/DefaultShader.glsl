#type vertex
#version 430


layout(location = 0) in vec4  a_Color;
layout(location = 1) in vec3  a_Position;
layout(location = 2) in vec2  a_TexCoord;
layout(location = 3) in float a_TextureID;



out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TextureID;

uniform mat4 u_ViewProjection;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TextureID = a_TextureID;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 430

#define PI 3.141592

// size of a square in pixel


in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TextureID;

// Temporary
uniform sampler2D u_Texture[32];
out vec4 FragColor;

void main()
{
	FragColor = texture(u_Texture[int(v_TextureID)], v_TexCoord) * v_Color;
}


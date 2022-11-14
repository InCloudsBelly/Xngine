#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TextureCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
	TexCoord = a_TextureCoord;
}

#type fragment
#version 450 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ComTexture;
uniform sampler2D HBAOBlur;

void main()
{
    float AO = texture(HBAOBlur,TexCoord).r;

	FragColor = vec4(texture(ComTexture, TexCoord).rgb * AO,1);
}

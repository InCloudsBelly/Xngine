#type vertex
#version 450 core

layout (location = 0) in vec2 _Position;
layout (location = 1) in vec2 _TexCoords;

out vec2 v2f_TexCoords;

void main()
{
	gl_Position = vec4(_Position, 0.0, 1.0);
	v2f_TexCoords = _TexCoords;
}


#type fragment
#version 450 core

in  vec2 v2f_TexCoords;
layout (location = 0) out vec4 FragColor_;

uniform sampler2D u_HBAOTexture;


void main()
{
    vec2 TexelSize = 1.0 / vec2(textureSize(u_HBAOTexture, 0));
    float Result = 0.0;
    for (int x = -3; x < 4; ++x) 
    {
        for (int y = -3; y < 4; ++y ) 
        {
            vec2 Offset = vec2(float(x), float(y)) * TexelSize;
            Result += texture(u_HBAOTexture, v2f_TexCoords + Offset).r;
        }
    }
    FragColor_ = vec4(Result / (7.0 * 7.0),0,0,1.0);
}
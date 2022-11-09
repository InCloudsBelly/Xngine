#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;
layout(location = 5) in int a_EntityID;

// Animation
layout(location = 6) in ivec4 a_BoneIds;
layout(location = 7) in vec4 a_BoneWeights;

uniform bool u_Animated;

struct VertexOutput
{
	vec3 Normal;
	vec3 WorldPos;
	vec2 TexCoord;
};

layout (location = 0) out VertexOutput Output;
layout (location = 3) out flat int v_EntityID;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];


void main()
{
    Output.TexCoord = a_TexCoord;

    if (u_Animated)
    {
        mat4 boneTransform = finalBonesMatrices[a_BoneIds[0]] * a_BoneWeights[0];
        boneTransform += finalBonesMatrices[a_BoneIds[1]] * a_BoneWeights[1];
        boneTransform += finalBonesMatrices[a_BoneIds[2]] * a_BoneWeights[2];
        boneTransform += finalBonesMatrices[a_BoneIds[3]] * a_BoneWeights[3];

        vec4 localPosition = boneTransform * vec4(a_Pos, 1.0);

        vec4 WorldPos = model * localPosition;
        Output.WorldPos = WorldPos.xyz;
        Output.Normal = mat3(boneTransform) * a_Normal;
        v_EntityID = a_EntityID;
        gl_Position = u_ViewProjection * WorldPos;
    }
    else
    {
        Output.WorldPos = vec3(model * vec4(a_Pos, 1.0));
        Output.Normal = mat3(model) * a_Normal;
        v_EntityID = a_EntityID;

        gl_Position = u_ViewProjection * vec4(Output.WorldPos, 1.0);
    }
}

#type fragment
#version 450 core

out vec4 _Albedo;
out vec4 _Position;
out vec4 _Normal;

struct VertexOutput
{
	vec3 Normal;
	vec3 WorldPos;
	vec2 TexCoord;
};

layout (location = 0) in VertexOutput Input;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;


// --------------------------PBR Function--------------------------------------

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, Input.TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(Input.WorldPos);
    vec3 Q2  = dFdy(Input.WorldPos);
    vec2 st1 = dFdx(Input.TexCoord);
    vec2 st2 = dFdy(Input.TexCoord);

    vec3 N   = normalize(Input.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


void main()
{
    vec4 _Albedo = vec4(pow(texture(albedoMap, Input.TexCoord).rgb, vec3(2.2)),1.0);
    _Albedo = vec4(1.0, 1.0, 0, 0);
    vec4 _Normal = vec4(getNormalFromMap(),1.0);
    vec4 _Position = vec4(Input.WorldPos,1.0);
}
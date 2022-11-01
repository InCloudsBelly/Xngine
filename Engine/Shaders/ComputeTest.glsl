#type compute
#version 450 core

#define LOCAL_GROUP_SIZE 16
layout(local_size_x = LOCAL_GROUP_SIZE, local_size_y = LOCAL_GROUP_SIZE) in;

layout(binding = 0, rgba8) uniform image2D data;

void main()
{
	imageStore(data, ivec2(gl_GlobalInvocationID.xy).xy, vec4(1.0,1.0,0.0, 1.0));
}


#version 430 core

layout (local_size_x = 1, local_size_y = 1,local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform image2D input_image;
layout (rgba32f, binding = 1) uniform image2D output_image;


void main(void)
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    vec4 data = imageLoad(input_image, pos); 
    barrier();
    data.y = data.y -0.01;
    imageStore(output_image, pos.xy, data);
}
#version 440

layout (local_size_x = 16, local_size_y = 16) in;

layout (rgba32f, binding = 0) uniform image2D input_image;
layout (rgba32f, binding = 1) uniform image2D output_image;
layout(r32i, binding = 2) uniform iimage2D int_image;


void main(void)
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    imageAtomicAdd(int_image, pos.xy, -2);
    imageStore(output_image, pos.xy, vec4(1,1,1,1));
}
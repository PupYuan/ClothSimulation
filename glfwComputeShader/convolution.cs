#version 440

layout (local_size_x = 16, local_size_y = 16) in;

// 传递卷积核
uniform float v[4];

layout (rgba32f, binding = 0) uniform image2D input_image;
layout (rgba32f, binding = 1) uniform image2D output_image;

shared ivec4 scanline[16][16];

void main(void)
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    vec4 temp = vec4(imageLoad(input_image, pos));
    temp = temp * 1000;
    scanline[pos.x][pos.y] = ivec4(temp);
    //vec4 data = imageLoad(input_image, pos); 
    barrier();
    ivec2 halfpos = ivec2((pos.x)/2,(pos.y)/2);
    atomicAdd(scanline[halfpos.x][halfpos.y].x, 222);
    atomicAdd(scanline[halfpos.x][halfpos.y].y, 222);
    atomicAdd(scanline[halfpos.x][halfpos.y].z, 222);
    atomicAdd(scanline[halfpos.x][halfpos.y].w, 222);
    vec4 data = scanline[pos.x][pos.y]/1000.0f;
    //data.r = 0 + data.r;
    //data.g = 1 + data.g;
    //data.b = 2 + data.b;
    //data.a = data.a;
    imageStore(output_image, pos.xy, data);
}
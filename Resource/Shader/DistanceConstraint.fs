#version 430 core
layout (local_size_x = 1, local_size_y = 1) in;

layout (rgba32f, binding = 1) uniform image2D input_Pos_image;
layout (rgba32f, binding = 2) uniform image2D input_Index_image;
layout (rgba32f, binding = 3) uniform image2D output_PosDelta_image;
layout (rgba32f, binding = 4) uniform image2D output_Index_image;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	ivec4 index = imageLoad(input_Index_image,pos);

	vec2 index1 = index.xy;
	vec2 index2 = index.zw;
	vec3 x_1 = imageLoad(input_Pos_image,index1);
	vec3 x_2 = imageLoad(input_Pos_image,index2);

	//经历了约束之后得到


   //存储
   imageStore(output_PosDelta_image, 2*pos , x_1); 
   imageStore(output_PosDelta_image, 2*pos + vec2(1,0) , x_2); 
   imageStore(output_Index_image, 2*pos ,index1);
   imageStore(output_Index_image, 2*pos + vec2(1,0) ,index2);
}
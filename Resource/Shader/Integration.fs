#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform float mass;					//point's mass
uniform float dt;						//timeStep
uniform int width;			//size of position texture
uniform vec3  gravity;				//gravitational force
uniform float global_dampening;	//default velocity damping

layout (rgba32f, binding = 0) uniform image2D input_Pos;
layout (rgba32f, binding = 1) uniform image2D input_LastPos;
layout (rgba32f, binding = 2) uniform image2D output_Pos;
layout (rgba32f, binding = 3) uniform image2D output_LastPos;



void main(void)
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	vec3 CurPos = imageLoad(input_Pos,pos).xyz;
	vec3 LastPos = imageLoad(input_LastPos,pos).xyz;

	vec3 vel	= (CurPos - LastPos) / dt;	// calc. velocity according to verlet integration
	
	float mymass = mass; 
	//末端节点固定
	if ((pos.x==0&&pos.y==0) || (pos.x==width-1&&pos.y==0))
		 mymass = 0.0;

    vec3 force = gravity;

	vec3 acc;
	if(mymass == 0) 		
	   acc = vec3(0);	//prevent the explosion due to divide by zero
	else
	   acc = force/mymass;

	vel = vel * global_dampening;
	vel = vel + acc*dt;
	// verlet integration
	vec3 tmp = CurPos;
	CurPos = CurPos + vel * dt;
	LastPos = tmp;

	//输出
    imageStore(output_Pos, pos.xy, vec4(CurPos,1.0));
	imageStore(output_LastPos, pos.xy, vec4(LastPos,1.0));
}
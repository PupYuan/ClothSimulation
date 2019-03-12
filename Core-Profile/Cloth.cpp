#include "Cloth.h"

/* This is a important constructor for the entire system of particles and constraints*/
Cloth::Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles
	vertices.resize(num_particles_width*num_particles_height * 6);
	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			vec3 pos = vec3(width * (x / (float)num_particles_width),
				-height * (y / (float)num_particles_height),
				0);

			particles[x*num_particles_height + y] = Particle(pos); // insert particle in column x at y'th row
			Particle *particle = &particles[x*num_particles_height + y];
			//同样保存一份在内存中：
			//顶点位置,并且传递地址给particle
			//
			vertices[6*(x*num_particles_height + y)] = pos.x;
			particle->x = &vertices[6 * (x*num_particles_height+y)];

			vertices[6 * (x*num_particles_height+y)+1] = pos.y;
			particle->y = &vertices[6 * (x*num_particles_height+y) + 1];

			vertices[6 * (x*num_particles_height+y) + 2] = pos.z;
			particle->z = &vertices[6 * (x*num_particles_height+y) + 2];
			//顶点法线
			vertices[6 * (x*num_particles_height+y) + 3] = 0.0f;
			vertices[6 * (x*num_particles_height+y) + 4] = 0.0f;
			vertices[6 * (x*num_particles_height+y) + 5] = 1.0f;
		}
	}
	//填充索引数据到indices中
	for (int x = 0; x < num_particles_width - 1; x++)
	{
		for (int y = 0; y < num_particles_height - 1; y++)
		{
			indices.push_back((x)*num_particles_height + y + 1);
			indices.push_back((x + 1)*num_particles_height + y + 1);
			indices.push_back((x)*num_particles_height + y);

			indices.push_back((x + 1)*num_particles_height + y + 1);
			indices.push_back((x + 1)*num_particles_height + y);
			indices.push_back((x)*num_particles_height + y);
		}
	}

	// making the upper left most three and right most three particles unmovable
	for (int i = 0; i < 3; i++)
	{
		getParticle(0 + i, 0)->offsetPos(vec3(0.5, 0.0, 0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
		getParticle(0 + i, 0)->makeUnmovable();

		getParticle(0 + i, 0)->offsetPos(vec3(-0.5, 0.0, 0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
		getParticle(num_particles_width - 1 - i, 0)->makeUnmovable();
	}
	//初始化gl缓存对象
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), NULL, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*vertices.size(), &vertices[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Cloth::drawShaded()
{
	// reset normals (which where written to last frame)
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).resetNormal();
	}

	//create smooth per particle normals by adding up all the (hard) triangle normals that each particle is part of
	for (int x = 0; x < num_particles_width - 1; x++)
	{
		for (int y = 0; y < num_particles_height - 1; y++)
		{
			vec3 normal = calcTriangleNormal(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1));
			getParticle(x + 1, y)->addToNormal(normal);
			getParticle(x, y)->addToNormal(normal);
			getParticle(x, y + 1)->addToNormal(normal);

			normal = calcTriangleNormal(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1));
			getParticle(x + 1, y + 1)->addToNormal(normal);
			getParticle(x + 1, y)->addToNormal(normal);
			getParticle(x, y + 1)->addToNormal(normal);
		}
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*vertices.size(), &vertices[0]);
	glDrawElements(GL_TRIANGLES, 6 * (num_particles_height - 1)*(num_particles_width - 1), GL_UNSIGNED_INT, 0);
}

inline glm::vec3 GetVerletVelocity(glm::vec3 x_i, glm::vec3 xi_last, float dt) {
	return  (x_i - xi_last) / dt;
}

void Cloth::addForce(const vec3 direction)
{
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).addForce(direction); // add the forces to each particle
	}
}

//物理模拟
void Cloth::timeStep(float dt) {
	//摩擦力
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		glm::vec3 V = GetVerletVelocity((*particle).getPos(), (*particle).getLastPos(), dt);
		(*particle).addForce(DEFAULT_DAMPING * V);
	}

	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).timeStep(dt);
	}
}
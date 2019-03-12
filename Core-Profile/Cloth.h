#pragma once
#include <ClothSimulation\Particles.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Cloth
{
private:
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	// total number of particles is num_particles_width*num_particles_height

	std::vector<Particle> particles; // all particles that are part of this cloth

	Particle* getParticle(int x, int y) { return &particles[y*num_particles_width + x]; }
	//在内存中一份顶点数据
	std::vector<float>vertices;
	std::vector<unsigned int>indices;
	//绘制相关
	unsigned int VBO, VAO, EBO;

public:

	/* This is a important constructor for the entire system of particles and constraints*/
	Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
	{
		particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles

		// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
		for (int x = 0; x < num_particles_width; x++)
		{
			for (int y = 0; y < num_particles_height; y++)
			{
				vec3 pos = vec3(width * (x / (float)num_particles_width),
					-height * (y / (float)num_particles_height),
					0);
				
				particles[y*num_particles_width + x] = Particle(pos); // insert particle in column x at y'th row
				//同样保存一份在内存中：
				//顶点位置
				vertices.push_back(pos.x);
				vertices.push_back(pos.y);
				vertices.push_back(pos.z);
				//顶点法线
				vertices.push_back(0.0f);
				vertices.push_back(0.0f);
				vertices.push_back(1.0f);
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

				indices.push_back((x + 1)*num_particles_height + y+1);
				indices.push_back((x+1)*num_particles_height + y);
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
	~Cloth() {
		// optional: de-allocate all resources once they've outlived their purpose:
	    // ------------------------------------------------------------------------
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	vec3 calcTriangleNormal(Particle *p1, Particle *p2, Particle *p3)
	{
		vec3 pos1 = p1->getPos();
		vec3 pos2 = p2->getPos();
		vec3 pos3 = p3->getPos();

		vec3 v1 = pos2 - pos1;
		vec3 v2 = pos3 - pos1;

		return cross(v1, v2);
		
	}
	/* drawing the cloth as a smooth shaded (and colored according to column) OpenGL triangular mesh
	Called from the display() method
	The cloth is seen as consisting of triangles for four particles in the grid as follows:

	(x,y)   *--* (x+1,y)
			| /|
			|/ |
	(x,y+1) *--* (x+1,y+1)

	*/
	void drawShaded()
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
		glDrawElements(GL_TRIANGLES, 6* (num_particles_height-1)*(num_particles_width-1), GL_UNSIGNED_INT, 0);
	}
};

#include <ClothSimulation\Cloth.h>
#include <ClothSimulation\SceneManager.h>
float	KsStruct = 50.0f, KdStruct = -0.25f;
float	KsShear = 50.0f, KdShear = -0.25f;
float	KsBend = 50.0f, KdBend = -0.25f;
int vertice_data_length = 8;
#define CHECK_GL_ERRORS assert(glGetError()==GL_NO_ERROR);

/* This is a important constructor for the entire system of particles and constraints*/
Cloth::Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles
	vertices.resize(num_particles_width*num_particles_height * vertice_data_length);
	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for (int y = 0; y < num_particles_height; y++)
	{
		for (int x = 0; x < num_particles_width; x++)
		{
			//vec3 pos = vec3(width * (x / (float)num_particles_width),
			//	-height * (y / (float)num_particles_height),
			//	0);
			vec3 pos = glm::vec3(width * (x / (float)num_particles_width), -height * (y / (float)num_particles_height),0 );

			particles[y*num_particles_width + x] = Particle(pos); // insert particle in column x at y'th row
			Particle *particle = &particles[y*num_particles_width + x];
			//同样保存一份在内存中：
			//顶点位置,并且传递地址给particle
			vertices[vertice_data_length*(y*num_particles_width + x)] = pos.x;
			particle->x = &vertices[vertice_data_length * (y*num_particles_width + x)];

			vertices[vertice_data_length * (y*num_particles_width + x)+1] = pos.y;
			particle->y = &vertices[vertice_data_length * (y*num_particles_width + x) + 1];

			vertices[vertice_data_length * (y*num_particles_width + x) + 2] = pos.z;
			particle->z = &vertices[vertice_data_length * (y*num_particles_width + x) + 2];
			//顶点法线，并且传递地址给particle
			vertices[vertice_data_length * (y*num_particles_width + x) + 3] = 0.0f;
			particle->normal_x = &vertices[vertice_data_length * (y*num_particles_width + x) + 3];

			vertices[vertice_data_length * (y*num_particles_width + x) + 4] = 0.0f;
			particle->normal_y = &vertices[vertice_data_length * (y*num_particles_width + x) + 4];

			vertices[vertice_data_length * (y*num_particles_width + x) + 5] = 1.0f;
			particle->normal_z = &vertices[vertice_data_length * (y*num_particles_width + x) + 5];
			//纹理坐标
			//x
			float texture_val_x;
			if(x%2==0)
				texture_val_x = 0.0f;//左
			else 
				texture_val_x = 1.0f;//右边
			vertices[vertice_data_length * (y*num_particles_width + x) + 6] = texture_val_x;
			//y
			float texture_val_y;
			if (y % 2 == 0)
				texture_val_y = 1.0f;//上
			else
				texture_val_y = 0.0f;//左上
			vertices[vertice_data_length * (y*num_particles_width + x) + 7] = texture_val_y;
			

		}
	}
	//填充索引数据到indices中
	for (int y = 0; y < num_particles_height - 1; y++)
	{
		for (int x = 0; x < num_particles_width - 1; x++)
		{
			indices.push_back((y)*num_particles_width + x + 1);
			indices.push_back((y + 1)*num_particles_width + x + 1);
			indices.push_back((y)*num_particles_width + x);

			indices.push_back((y + 1)*num_particles_width + x + 1);
			indices.push_back((y + 1)*num_particles_width + x);
			indices.push_back((y)*num_particles_width + x);
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertice_data_length * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertice_data_length * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertice_data_length * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Constraints
		// Setup springs
	// 添加Springs
	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			// Structure Springs
			if (x + 1 < num_particles_width)
				AddSpring(getParticle(x, y), getParticle(x + 1, y), KsStruct, KdStruct);
			if (y + 1 < num_particles_height)
				AddSpring(getParticle(x, y), getParticle(x, y + 1), KsStruct, KdStruct);

			//Shear Springs
			if (y + 1 < num_particles_height && x + 1 < num_particles_width) {
				AddSpring(getParticle(x, y), getParticle(x + 1, y + 1), KsShear, KdShear);
				AddSpring(getParticle(x + 1, y), getParticle(x, y + 1), KsShear, KdShear);
			}

			//Bending Springs
			if (x + 2 < num_particles_width)
				AddSpring(getParticle(x, y), getParticle(x + 2, y), KsBend, KdBend);
			if (y + 2 < num_particles_height)
				AddSpring(getParticle(x, y), getParticle(x, y + 2), KsBend, KdBend);
			if (y + 2 < num_particles_height && x + 2 < num_particles_width) {
				AddSpring(getParticle(x, y), getParticle(x + 2, y + 2), KsBend, KdBend);
				AddSpring(getParticle(x + 2, y), getParticle(x, y + 2), KsBend, KdBend);
			}
		}
	}

	renderShader = ResourcesManager::loadShader("ClothShader", "../Resource/Shader/Simple.vs", "../Resource/Shader/Simple.fs");
}
void Cloth::RenderCPU() {
	//场景的信息要单独抽离出来
	renderShader->use();
	renderShader->setMat4("projection", scene->projection);
	renderShader->setMat4("view", scene->view);
	glm::mat4 model1;
	//model1 = glm::translate(model1, glm::vec3(-6.0f, 2.0f, -5.0f)); // translate it down so it's at the center of the scene
	renderShader->setMat4("model", model1);

	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene->diffuseMap);

	renderShader->setVec3("light.position", scene->light.lightPos);
	renderShader->setVec3("viewPos", scene->camera.Position);
	renderShader->setVec3("light.ambient", scene->light.ambientColor);
	renderShader->setVec3("light.diffuse", scene->light.diffuseColor);
	renderShader->setVec3("light.specular", 0.2f, 0.2f, 0.2f);

	renderShader->setFloat("material.shininess", 16.0f);
	renderShader->setVec3("material.specular", vec3(0.2f, 0.2f, 0.2f));

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
void Cloth::RenderGPU() {
	//调整正交矩阵，渲染到2D平面
	glViewport(0, 0, texture_size_x, texture_size_y);
	CHECK_GL_ERRORS
		glBeginQuery(GL_TIME_ELAPSED, t_query);
	for (int i = 0; i < NUM_ITER; i++) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID[writeID]);
		glDrawBuffers(2, mrt);

		CHECK_GL_ERRORS
			glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, attachID[2 * readID]);

		CHECK_GL_ERRORS
			glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, attachID[2 * readID + 1]);

		glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//verletShader.use();
		//把数据渲染到纹理上


		//swap read/write pathways
		int tmp = readID;
		readID = writeID;
		writeID = tmp;
	}

	CHECK_GL_ERRORS

		//read back the results into the VBO
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID[readID]);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, vboID);
	glReadPixels(0, 0, texture_size_x, texture_size_y, GL_RGBA, GL_FLOAT, 0);
	glEndQuery(GL_TIME_ELAPSED);

	// get the query result
	glGetQueryObjectui64v(t_query, GL_QUERY_RESULT, &elapsed_time);
	delta_time = elapsed_time / 1000000.0f;
	CHECK_GL_ERRORS

		glReadBuffer(GL_NONE);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	//reset default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);

	//restore the rendering modes and viewport
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glViewport(0, 0, num_particles_width, num_particles_height);

	CHECK_GL_ERRORS

		//renderShader.Use();
		//glBindBuffer(GL_ARRAY_BUFFER, vboID);
		//glVertexPointer(4, GL_FLOAT, 0, 0);
		//glEnableClientState(GL_VERTEX_ARRAY);
		////draw plygons
		//glUniform4fv(renderShader("color"), 1, vWhite);
		//glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, &(indices[0]));

		////draw points			
		//glUniform4fv(renderShader("color"), 1, vRed);
		//glUniform1i(renderShader("selected_index"), selected_index);
		//glDrawArrays(GL_POINTS, 0, total_points);
		//glUniform1i(renderShader("selected_index"), -1);
		//glDisableClientState(GL_VERTEX_ARRAY);
		//renderShader.UnUse();
}


void Cloth::drawShaded()
{
	switch (current_mode) {
	case CPU:
		RenderCPU();
		break;

	case GPU:
		RenderGPU();
		break;
	}
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

	std::vector<Spring>::iterator Spring;
	for (Spring = Springs.begin(); Spring != Springs.end(); Spring++)
	{
		(*Spring).satisfySpring(dt); // satisfy Spring.
	}

	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).timeStep(dt);
	}
}

void Cloth::AddSpring(Particle* a, Particle* b, float ks, float kd) {
	Spring spring;
	spring.p1 = a;
	spring.p2 = b;
	spring.Ks = ks;
	spring.Kd = kd;
	glm::vec3 deltaP = vec3(a->getPos() - b->getPos());
	spring.restDistance = sqrt(glm::dot(deltaP, deltaP));
	Springs.push_back(spring);
}
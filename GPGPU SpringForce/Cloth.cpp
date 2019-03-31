#include <ClothSimulation\Cloth.h>
#include <ClothSimulation\SceneManager.h>
float	KsStruct = 50.75f, KdStruct = -0.25f;
float	KsShear = 50.75f, KdShear = -0.25f;
float	KsBend = 50.95f, KdBend = -0.25f;
int vertice_data_length = 8;
#define CHECK_GL_ERRORS assert(glGetError()==GL_NO_ERROR);

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

/* This is a important constructor for the entire system of particles and constraints*/
Cloth::Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	X.resize(total_points);
	X_last.resize(total_points);

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
			//vec3 pos = glm::vec3(width * (x / (float)num_particles_width), -height * (y / (float)num_particles_height),0 );
			//vec3 pos = glm::vec3(((float(x) / (num_particles_width - 1)) * 2 - 1)* width/2, width + 1, ((float(y) / (num_particles_height - 1))* height));
			vec3 pos = glm::vec3(((float(x) / (num_particles_width - 1)) * 2 - 1)* width / 2, 1.0, ((float(y) / (num_particles_height - 1))* height));
			particles[y*num_particles_width + x] = Particle(pos); // insert particle in column x at y'th row
			Particle *particle = &particles[y*num_particles_width + x];

			X[(y*num_particles_width + x)] = vec4(pos, 1);
			X_last[(y*num_particles_width + x)] = vec4(pos, 1);
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

	//renderShader = ResourcesManager::loadShader("ClothShader", "../Resource/Shader/Simple.vs", "../Resource/Shader/Simple.fs");
	renderShader = ResourcesManager::loadShader("renderShader", "render.vs", "render.fs");
	verletShader = ResourcesManager::loadShader("verletShader","verlet.vs", "verlet.fs");
	verletShader->use();
	verletShader->setFloat("DEFAULT_DAMPING", DEFAULT_DAMPING);
	verletShader->setFloat("mass", 1);
	verletShader->setVec3("gravity", glm::vec3(0.0f, -0.0981f, 0.0f));
	verletShader->setFloat("dt", 1.0f / 60.0f);
	verletShader->setFloat("texsize_x", num_particles_width);
	verletShader->setFloat("texsize_y", num_particles_height);
	verletShader->setFloat("KsStruct", KsStruct);
	verletShader->setFloat("KdStruct", KdStruct);
	verletShader->setFloat("KsShear", KsShear);
	verletShader->setFloat("KdShear", KdShear);
	verletShader->setFloat("KsBend", KsBend);
	verletShader->setFloat("KdBend", KdBend);
	verletShader->setVec2("inv_cloth_size", float(width) /(num_particles_width-1), float(height) / (num_particles_height-1));
	verletShader->setVec2("step", 1.0f / (num_particles_width-1.0f), 1.0f / (num_particles_height-1.0f));
	//Init for GPGPU

	const int size = num_particles_width * num_particles_height * 4 * sizeof(float);
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboID);
	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	_data[0] = &X[0].x;
	_data[1] = &X_last[0].x;
	// screen quad VAO
	//unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// framebuffer configuration
	// -------------------------
	//unsigned int framebuffer;
	glGenFramebuffers(2, fboID);
	glGenTextures(4, attachID);
	for (int j = 0; j < 2; j++) {//两个帧缓冲，用于输入和输出交替
		glBindFramebuffer(GL_FRAMEBUFFER, fboID[j]);
		for (int i = 0; i < 2; i++) {//两块纹理，用于verlet积分的当前位置和过去位置
			glBindTexture(GL_TEXTURE_2D, attachID[i + 2 * j]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, num_particles_width, num_particles_height, 0, GL_RGBA, GL_FLOAT, _data[i]); // NULL = Empty texture

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, mrt[i], GL_TEXTURE_2D, attachID[i + 2 * j], 0);
		}
	}
	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status == GL_FRAMEBUFFER_COMPLETE) {
		printf("FBO setup succeeded.");
	}
	else {
		printf("Problem with FBO setup.");
	}
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
	glViewport(0, 0, num_particles_width, num_particles_height);
	// render
	// ------
	// bind to framebuffer and draw scene as we normally would to color texture 
	for (int i = 0; i < NUM_ITER; i++) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID[writeID]);
		glDisable(GL_DEPTH_TEST);

        // make sure we clear the framebuffer's content
        glClear(GL_COLOR_BUFFER_BIT);
		glDrawBuffers(2, mrt);

		verletShader->use();
		glBindVertexArray(quadVAO);

		CHECK_GL_ERRORS
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, attachID[2 * readID]);

		CHECK_GL_ERRORS
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, attachID[2 * readID + 1]);
	    glDrawArrays(GL_TRIANGLES, 0, 6);


		//swap read/write pathways
		int tmp = readID;
		readID = writeID;
		writeID = tmp;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID[readID]);
	//将framebuffer中的颜色附件读取进
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, vboID);
	glReadPixels(0, 0, num_particles_width, num_particles_height, GL_RGBA, GL_FLOAT, 0);

	//重置状态
	glReadBuffer(GL_NONE);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	//渲染
	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, scene->SCR_WIDTH, scene->SCR_HEIGHT);
	renderShader->use();
	renderShader->setMat4("projection", scene->projection);
	renderShader->setMat4("view", scene->view);
	glm::mat4 model1;
	//model1 = glm::translate(model1, glm::vec3(-6.0f, 2.0f, -5.0f)); // translate it down so it's at the center of the scene
	renderShader->setMat4("model", model1);
	glPointSize(5);
	glBindVertexArray(vaoID);
	glDrawArrays(GL_POINTS, 0, num_particles_width * num_particles_height);
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
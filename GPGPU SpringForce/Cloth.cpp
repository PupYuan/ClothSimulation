#include <ClothSimulation\Cloth.h>
#include <ClothSimulation\SceneManager.h>
float	KsStruct = 50.75f, KdStruct = -0.25f;
float	KsShear = 50.75f, KdShear = -0.25f;
float	KsBend = 50.95f, KdBend = -0.25f;
float kBend = 0.5f;
float kStretch = 0.25f;
int vertice_data_length = 8;
#define CHECK_GL_ERRORS assert(glGetError()==GL_NO_ERROR);

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	//-1.0f, -1.0f,  0.0f, 0.0f,
	//1.0f, -1.0f,  1.0f, 0.0f,
	//1.0f,  1.0f,  1.0f, 1.0f,
	//1.0f,  1.0f,  1.0f, 1.0f,
	//-1.0f,  1.0f,  0.0f, 1.0f,
	//-1.0f, -1.0f,  0.0f, 0.0f,
	// positions        // texture Coords
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f,  1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
};

void Cloth::InitCPU() {
	particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles
	vertices.resize(num_particles_width*num_particles_height * vertice_data_length);
	Ri.resize(total_points);
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
			vec3 pos = glm::vec3(((float(x) / (num_particles_width - 1)) * 2 - 1)* width / 2, 1, ((float(y) / (num_particles_height - 1))* height));
			particles[y*num_particles_width + x] = Particle(pos,1.0f/ (num_particles_width * num_particles_height)); // insert particle in column x at y'th row
			Particle *particle = &particles[y*num_particles_width + x];

			//顶点位置,并且传递地址给particle
			vertices[vertice_data_length*(y*num_particles_width + x)] = pos.x;
			particle->x = &vertices[vertice_data_length * (y*num_particles_width + x)];

			vertices[vertice_data_length * (y*num_particles_width + x) + 1] = pos.y;
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
			if (x % 2 == 0)
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

	renderShader = ResourcesManager::loadShader("ClothShader", "../Resource/Shader/Simple.vs", "../Resource/Shader/Simple.fs");
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
	//for (int x = 0; x < num_particles_width; x++)
	//{
	//	for (int y = 0; y < num_particles_height; y++)
	//	{
	//		// Structure Springs
	//		if (x + 1 < num_particles_width)
	//			AddSpring(getParticle(x, y), getParticle(x + 1, y), KsStruct, KdStruct);
	//		if (y + 1 < num_particles_height)
	//			AddSpring(getParticle(x, y), getParticle(x, y + 1), KsStruct, KdStruct);

	//		//Shear Springs
	//		if (y + 1 < num_particles_height && x + 1 < num_particles_width) {
	//			AddSpring(getParticle(x, y), getParticle(x + 1, y + 1), KsShear, KdShear);
	//			AddSpring(getParticle(x + 1, y), getParticle(x, y + 1), KsShear, KdShear);
	//		}

	//		//Bending Springs
	//		if (x + 2 < num_particles_width)
	//			AddSpring(getParticle(x, y), getParticle(x + 2, y), KsBend, KdBend);
	//		if (y + 2 < num_particles_height)
	//			AddSpring(getParticle(x, y), getParticle(x, y + 2), KsBend, KdBend);
	//		if (y + 2 < num_particles_height && x + 2 < num_particles_width) {
	//			AddSpring(getParticle(x, y), getParticle(x + 2, y + 2), KsBend, KdBend);
	//			AddSpring(getParticle(x + 2, y), getParticle(x, y + 2), KsBend, KdBend);
	//		}
	//	}
	//}
	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			// Distance Constraints
			if (x + 1 < num_particles_width)
				AddConstraint(getParticle(x, y), getParticle(x + 1, y), kStretch);
			if (y + 1 < num_particles_height)
				AddConstraint(getParticle(x, y), getParticle(x, y + 1), kStretch);
			//Shear Springs
		    if (y + 1 < num_particles_height && x + 1 < num_particles_width) {
				AddConstraint(getParticle(x, y), getParticle(x + 1, y + 1), kStretch);
				AddConstraint(getParticle(x + 1, y), getParticle(x, y + 1), kStretch);
		    }
		}
	}
	for (int i = 0; i < num_particles_width; i++) {
		for (int j = 0; j < num_particles_height - 2; j++) {
			BendingConstraint2* constraint = new BendingConstraint2(getParticle(i, j), getParticle(i, j+1), getParticle(i, j+2), kBend);
			Constraints.push_back(constraint);
		}
	}
	//add horizontal constraints
	for (int i = 0; i < num_particles_width - 2; i++) {
		for (int j = 0; j < num_particles_height; j++) {
			BendingConstraint2* constraint = new BendingConstraint2(getParticle(i, j), getParticle(i+1, j), getParticle(i+2, j), kBend);
			Constraints.push_back(constraint);
		}
	}

	//for (int i = 0; i < num_particles_height - 1; ++i) {
	//	for (int j = 0; j < num_particles_width - 1; ++j) {
	//		int p1 = i * (num_particles_width) + j;
	//		int p2 = p1 + 1;
	//		int p3 = p1 + (num_particles_width);
	//		int p4 = p3 + 1;

	//		if ((j + i) % 2) {
	//			BendingConstraint2* constraint = new BendingConstraint2(&particles[p3], &particles[p2], &particles[p1], &particles[p4], kBend);
	//			Constraints.push_back(constraint);
	//		}
	//		else {
	//			BendingConstraint2* constraint = new BendingConstraint2(&particles[p4], &particles[p1], &particles[p3], &particles[p2], kBend);
	//			Constraints.push_back(constraint);
	//		}
	//	}
	//}
}

void Cloth::InitGPU() {
	X.resize(total_points);
	X_last.resize(total_points);
	Normal.resize(total_points);
	TexCoord.resize(total_points);
	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for (int y = 0; y < num_particles_height; y++)
	{
		for (int x = 0; x < num_particles_width; x++)
		{
			vec3 pos = glm::vec3(((float(x) / (num_particles_width - 1)) * 2 - 1)* width / 2, 1.0, ((float(y) / (num_particles_height - 1))* height));

			X[(y*num_particles_width + x)] = vec4(pos, 1);
			X_last[(y*num_particles_width + x)] = vec4(pos, 1);
			Normal[(y*num_particles_width + x)] = vec3(0, 0, 1);
			//纹理坐标
			//x
			vec2 temp(0, 0);
			float texture_val_x;
			if (x % 2 == 0)
				texture_val_x = 0.0f;//左
			else
				texture_val_x = 1.0f;//右边
			temp.x = texture_val_x;
			//y
			float texture_val_y;
			if (y % 2 == 0)
				texture_val_y = 1.0f;//上
			else
				texture_val_y = 0.0f;//左上
			temp.y = texture_val_y;
			TexCoord[(y*num_particles_width + x)] = temp;
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
	//renderShader = ResourcesManager::loadShader("ClothShader", "../Resource/Shader/Simple.vs", "../Resource/Shader/Simple.fs");
	renderShader = ResourcesManager::loadShader("renderShader", "render.vs", "render.fs");
	verletShader = ResourcesManager::loadShader("verletShader", "verlet.vs", "verlet.fs");
	verletShader->use();
	verletShader->setFloat("DEFAULT_DAMPING", DEFAULT_DAMPING);
	verletShader->setFloat("mass", 1.0f);
	verletShader->setVec3("gravity", glm::vec3(0.0f, -9.81f, 0.0f));
	verletShader->setFloat("dt", 1.0f / 60.0f);
	verletShader->setFloat("texsize_x", float(num_particles_width));
	verletShader->setFloat("texsize_y", float(num_particles_height));
	verletShader->setFloat("KsStruct", KsStruct);
	verletShader->setFloat("KdStruct", KdStruct);
	verletShader->setFloat("KsShear", KsShear);
	verletShader->setFloat("KdShear", KdShear);
	verletShader->setFloat("KsBend", KsBend);
	verletShader->setFloat("KdBend", KdBend);
	//verletShader->setVec2("inv_cloth_size", float(width) / (num_particles_width-1), float(height) / (num_particles_height-1));
	verletShader->setVec2("inv_cloth_size", 4.0 / 63.0, 4.0 / 63.0);
	//verletShader->setVec2("step", 1.0f / (num_particles_width - 1.0f), 1.0f / (num_particles_height - 1.0f));
	verletShader->setVec2("step", 1.0f / 63.0f, 1.0f / 63.0f);
	//Init for GPGPU

	const int size = num_particles_width * num_particles_height *4* sizeof(float);
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &EBO);
	glBindVertexArray(vaoID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_DYNAMIC_DRAW);
	//顶点位置属性
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	//顶点法线属性
	glGenBuffers(1, &vboID2);
	glBindBuffer(GL_ARRAY_BUFFER, vboID2);
	glBufferData(GL_ARRAY_BUFFER, num_particles_width * num_particles_height * 3 * sizeof(float), &Normal[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	//顶点纹理属性
	glGenBuffers(1, &vboID3);
	glBindBuffer(GL_ARRAY_BUFFER, vboID3);
	glBufferData(GL_ARRAY_BUFFER, num_particles_width * num_particles_height * 2 * sizeof(float), &TexCoord[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

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
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID[j]);
		for (int i = 0; i < 2; i++) {//两块纹理，用于verlet积分的当前位置和过去位置
			glBindTexture(GL_TEXTURE_2D, attachID[i + 2 * j]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, num_particles_width, num_particles_height, 0, GL_RGBA, GL_FLOAT, _data[i]); // NULL = Empty texture

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, mrt[i], GL_TEXTURE_2D, attachID[i + 2 * j], 0);
		}
	}
	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status == GL_FRAMEBUFFER_COMPLETE) {
		printf("FBO setup succeeded.");
	}
	else {
		printf("Problem with FBO setup.");
	}
	CHECK_GL_ERRORS
}
/* This is a important constructor for the entire system of particles and constraints*/
Cloth::Cloth(float _width, float _height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	width = _width;
	height = _height;
	//glfwSwapInterval(0);
	switch (current_mode) {
	case CPU:
		InitCPU();
		break;

	case GPU:
		InitGPU();
		break;
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
	CHECK_GL_ERRORS
	glViewport(0, 0, num_particles_width, num_particles_height);
	// render
	// ------
	// bind to framebuffer and draw scene as we normally would to color texture 
	for (int i = 0; i < NUM_ITER; i++) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID[writeID]);
		glDrawBuffers(2, mrt);

		CHECK_GL_ERRORS
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, attachID[2 * readID]);

		CHECK_GL_ERRORS
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, attachID[2 * readID + 1]);

		glDisable(GL_DEPTH_TEST);
		// make sure we clear the framebuffer's content
		glClear(GL_COLOR_BUFFER_BIT);

		verletShader->use();
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	    //glDrawArrays(GL_TRIANGLES, 0, 6);


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

	CHECK_GL_ERRORS
	//重置状态
	glReadBuffer(GL_NONE);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	//渲染
	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glViewport(0, 0, scene->SCR_WIDTH, scene->SCR_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
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

	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, 6 * (num_particles_height - 1)*(num_particles_width - 1), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_POINTS, 0, num_particles_width * num_particles_height);
	CHECK_GL_ERRORS
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
	glm::vec3 Xcm = glm::vec3(0);
	glm::vec3 Vcm = glm::vec3(0);
	float sumM = 0;
	//摩擦力
	std::vector<Particle>::iterator particle;
	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		glm::vec3 V = GetVerletVelocity((*particle).getPos(), (*particle).getLastPos(), dt);
        //(*particle).addForce(DEFAULT_DAMPING * V);
        //直接按比例减少速度模拟摩擦力
		//glm::vec3 V = particle->getVelocity();
        V *= global_dampening;
		vec3 acc = particle->getAcceleration();
		V = V + acc * dt;
        particle->setVelocity(V);

		Xcm += ((*particle).getPos() * (*particle).getMass());
		Vcm += ((*particle).getVelocity() * (*particle).getMass());
		sumM += (*particle).getMass();
	}
	Xcm /= sumM;
	Vcm /= sumM;
	glm::mat3 I = glm::mat3(1);
	glm::vec3 L = glm::vec3(0);
	glm::vec3 w = glm::vec3(0);//angular velocity
	int i = 0;
	for (particle = particles.begin(); particle != particles.end(); particle++) {
		vec3 Xi = particle->getCurrentPos();
		Ri[i] = (Xi - Xcm);
		L += cross(Ri[i], particle->getVelocity() * particle->getMass());

		glm::mat3 tmp = glm::mat3(0, -Ri[i].z, Ri[i].y,
			Ri[i].z, 0, -Ri[i].x,
			-Ri[i].y, Ri[i].x, 0);
		I += (tmp*glm::transpose(tmp))*particle->getMass();
		i++;
	}
	w = glm::inverse(I)*L;
	i = 0;
	for (particle = particles.begin(); particle != particles.end(); i++, particle++) {
		glm::vec3 delVi = Vcm + glm::cross(w, Ri[i]) - particle->getVelocity();
		glm::vec3 V = particle->getVelocity();
		V += kDamp * delVi;
		particle->setVelocity(V);
	}

	for (particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).timeStep(dt);
	}
	for (size_t si = 0; si < Constraint::solver_iterations; ++si) {
		std::vector<Constraint*>::iterator constraint;
		for (constraint = Constraints.begin(); constraint != Constraints.end(); constraint++)
		{
			(*constraint)->satisfyConstraint(dt); // satisfy Spring.
		}
	}
	GroundCollision();
}
void Cloth::GroundCollision()
{
	for (auto particle = particles.begin(); particle != particles.end(); particle++)
	{
		particle->checkGround(-10);
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

void Cloth::AddConstraint(Particle* a, Particle* b, float k) {
	DistanceConstraint* constraint = new DistanceConstraint();
	constraint->particle1 = a;
	constraint->particle2 = b;
	constraint->k = k;
	constraint->k_prime = 1.0f - pow((1.0f - k), 1.0f / Constraint::solver_iterations);
	if (constraint->k_prime > 1.0)
		constraint->k_prime = 1.0;

	glm::vec3 deltaP = vec3(a->getPos() - b->getPos());
	constraint->restDistance = sqrt(glm::dot(deltaP, deltaP));
	Constraints.push_back(constraint);
}
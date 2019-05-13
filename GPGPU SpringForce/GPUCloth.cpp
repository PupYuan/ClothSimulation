#include <ClothSimulation\Cloth.h>
#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\util.h>
#define glCheckError() glCheckError_(__FILE__, __LINE__) 
float	KsStruct = 50.75f, KdStruct = -0.25f;
float	KsShear = 50.75f, KdShear = -0.25f;
float	KsBend = 50.95f, KdBend = -0.25f;

float quadVertices[] = {
	// positions        // texture Coords
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f,  1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
};

void PositionBasedUnit::CalcNormal(GLint attachID,int width,int height) {
	NormalCalcShader->use();
	for (int i = 0; i < 3; i++)
		glClearTexImage(NormalTexID[i], 0, GL_RED_INTEGER, GL_INT, &Null_X[0]);
	glFinish();
	//glBindImageTexture(0, attachID[2 * readID], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(0, attachID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, NormalTexID[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
	glBindImageTexture(2, NormalTexID[1], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
	glBindImageTexture(3, NormalTexID[2], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
	glBindImageTexture(4, NormalFloatTexID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//glDispatchCompute(num_particles_width - 1, num_particles_height - 1, 1);
	glDispatchCompute(width - 1, height - 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glFinish();
	//readNormal
	glBindBuffer(GL_PIXEL_PACK_BUFFER, NormalVboID[0]);
	glBindTexture(GL_TEXTURE_2D, NormalTexID[0]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, 0);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, NormalVboID[1]);
	glBindTexture(GL_TEXTURE_2D, NormalTexID[1]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, 0);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, NormalVboID[2]);
	glBindTexture(GL_TEXTURE_2D, NormalTexID[2]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, 0);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, NormalFloatVboID);
	glBindTexture(GL_TEXTURE_2D, NormalFloatTexID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, 0);
}

void PositionBasedUnit::InitVAOTex() {
	//Init for GPGPU
	const int size = num_particles_width * num_particles_height * 4 * sizeof(float);
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &EBO);
	glBindVertexArray(vaoID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_DYNAMIC_DRAW);
	//顶点位置属性
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	//顶点法线属性
	glGenBuffers(1, &NormalVboID[0]);
	glBindBuffer(GL_ARRAY_BUFFER, NormalVboID[0]);
	glBufferData(GL_ARRAY_BUFFER, num_particles_width * num_particles_height * 1 * sizeof(int), 0, GL_DYNAMIC_COPY);
	glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &NormalVboID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, NormalVboID[1]);
	glBufferData(GL_ARRAY_BUFFER, num_particles_width * num_particles_height * 1 * sizeof(int), 0, GL_DYNAMIC_COPY);
	glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &NormalVboID[2]);
	glBindBuffer(GL_ARRAY_BUFFER, NormalVboID[2]);
	glBufferData(GL_ARRAY_BUFFER, num_particles_width * num_particles_height * 1 * sizeof(int), 0, GL_DYNAMIC_COPY);
	glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);
	//顶点纹理属性
	glGenBuffers(1, &vboID3);
	glBindBuffer(GL_ARRAY_BUFFER, vboID3);
	glBufferData(GL_ARRAY_BUFFER, num_particles_width * num_particles_height * 2 * sizeof(float), &TexCoord[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(4);
	// 顶点纹理属性
	glGenBuffers(1, &NormalFloatVboID);
	glBindBuffer(GL_ARRAY_BUFFER, NormalFloatVboID);
	glBufferData(GL_ARRAY_BUFFER, num_particles_width * num_particles_height * 4 * sizeof(float), &Normal[0], GL_DYNAMIC_COPY);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(5);

	glGenTextures(3, NormalTexID);
	for (int i = 0; i < 3; i++) {
		setupIntTexture(NormalTexID[i], &Null_X[0], num_particles_width, num_particles_height);//attachID里面存放顶点数据
	}

	glGenTextures(1, &NormalFloatTexID);
	setupTexture(NormalFloatTexID, &Normal[0].x, num_particles_width, num_particles_height);
}
void GPUCloth::timeStep(float dt)
{
	//积分开始
	glBeginQuery(GL_TIME_ELAPSED, t_query);
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
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glFinish();
		verletShader->use();
		glUniform3fv(glGetUniformLocation(verletShader->ID, "sphere_pos"), 2, &scene->spherePos[0][0]);
		glUniform1fv(glGetUniformLocation(verletShader->ID, "radius"), 2, &scene->radius[0]);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glFinish();
		//swap read/write pathways
		int tmp = readID;
		readID = writeID;
		writeID = tmp;
	}
	CalcNormal(attachID[2 * readID], num_particles_width, num_particles_height);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID[readID]);
	//将framebuffer中的颜色附件读取进
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, vboID);
	glReadPixels(0, 0, num_particles_width, num_particles_height, GL_RGBA, GL_FLOAT, 0);
	glFinish();
	//积分结束
	glEndQuery(GL_TIME_ELAPSED);
	// get the query result
	glGetQueryObjectui64v(t_query, GL_QUERY_RESULT, &elapsed_time);
	delta_time = elapsed_time / 1000000.0f;

	CHECK_GL_ERRORS
	//重置状态
	glReadBuffer(GL_NONE);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glViewport(0, 0, scene->SCR_WIDTH, scene->SCR_HEIGHT);
	glEnable(GL_DEPTH_TEST);
}


GPUCloth::GPUCloth(float _width, float _height, int _num_particles_width, int _num_particles_height)
{
	num_particles_width = _num_particles_width;
	num_particles_height = _num_particles_height;
	total_points = (num_particles_width)*(num_particles_height);
	glGenQueries(1, &t_query);
	width = _width;
	height = _height;
	glfwSwapInterval(0);
	X.resize(total_points);
	X_last.resize(total_points);
	Normal.resize(total_points);
	TexCoord.resize(total_points);
	NormalX.resize(total_points, 0);
	NormalY.resize(total_points, 0);
	NormalZ.resize(total_points, 0);
	Null_X.resize(total_points, 0);
	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for (int y = 0; y < num_particles_height; y++)
	{
		for (int x = 0; x < num_particles_width; x++)
		{
			vec3 pos = glm::vec3(((float(x) / (num_particles_width - 1)) * 2 - 1)* width / 2, 1.0, ((float(y) / (num_particles_height - 1))* height));

			X[(y*num_particles_width + x)] = vec4(pos, 1);
			X_last[(y*num_particles_width + x)] = vec4(pos, 1);
			Normal[(y*num_particles_width + x)] = vec4(0, 0, 1,0);
			//纹理坐标
			vec2 temp;
			temp.x = x / (texDensityX);
			temp.y = y / (texDensityY);
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

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPointSize(5);

	renderShader = ResourcesManager::loadShader("GPU_renderShader", "render.vs", "render.fs");
	verletShader = ResourcesManager::loadShader("verletShader", "verlet.vs", "verlet.fs");
    NormalCalcShader = ResourcesManager::loadComputeShader("NormalCalculate", "NormalCalculate.fs");
	glCheckError();
	verletShader->use();
	verletShader->setFloat("DEFAULT_DAMPING", DEFAULT_DAMPING);
	verletShader->setFloat("mass", 1.0f);
	verletShader->setVec3("gravity", gravity);
	verletShader->setFloat("dt", 1.0f / 50.0f);
	verletShader->setFloat("texsize_x", float(num_particles_width));
	verletShader->setFloat("texsize_y", float(num_particles_height));
	verletShader->setFloat("KsStruct", KsStruct);
	verletShader->setFloat("KdStruct", KdStruct);
	verletShader->setFloat("KsShear", KsShear);
	verletShader->setFloat("KdShear", KdShear);
	verletShader->setFloat("KsBend", KsBend);
	verletShader->setFloat("KdBend", KdBend);
	verletShader->setVec2("inv_cloth_size", float(width) / (num_particles_width - 1), float(height) / (num_particles_height - 1));
	verletShader->setVec2("step", 1.0f / (num_particles_width - 1.0f), 1.0f / (num_particles_height - 1.0f));
	
	//初始化渲染所需的buffer
	InitVAOTex();

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
	glCheckError();
	// framebuffer configuration
	// -------------------------
	//unsigned int framebuffer;
	glGenFramebuffers(2, fboID);
	glGenTextures(4, attachID);
	for (int j = 0; j < 2; j++) {//两个帧缓冲，用于输入和输出交替
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID[j]);
		for (int i = 0; i < 2; i++) {//两块纹理，用于verlet积分的当前位置和过去位置
			glBindTexture(GL_TEXTURE_2D, attachID[i + 2 * j]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, num_particles_width, num_particles_height, 0, GL_RGBA, GL_FLOAT, _data[i]); // NULL = Empty texture


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

void GPUCloth::render()
{
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(vaoID);
	//glDrawArrays(GL_POINTS, 0, total_points);
	glDrawElements(GL_TRIANGLES, 6 * (num_particles_height - 1)*(num_particles_width - 1), GL_UNSIGNED_INT, 0);
	CHECK_GL_ERRORS
}
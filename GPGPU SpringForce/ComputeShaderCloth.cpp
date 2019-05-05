#include <ClothSimulation\Cloth.h>
#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\util.h>

void ComputeShaderCloth::readNormal() {
	glBindBuffer(GL_PIXEL_PACK_BUFFER, NormalVboID[0]);
	glBindTexture(GL_TEXTURE_2D, NormalTexID[0]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, 0);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, NormalVboID[1]);
	glBindTexture(GL_TEXTURE_2D, NormalTexID[1]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, 0);

	glBindBuffer(GL_PIXEL_PACK_BUFFER, NormalVboID[2]);
	glBindTexture(GL_TEXTURE_2D, NormalTexID[2]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, 0);
}
void ComputeShaderCloth::timeStep(float dt)
{
	CHECK_GL_ERRORS
	IntegrationShader->use();
	glFinish();
	glBindImageTexture(0, attachID[2 * readID], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, attachID[2 * readID + 1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(2, attachID[2 * writeID], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(3, attachID[2 * writeID + 1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(num_particles_width, num_particles_height, 1);
	glFinish();
	//swap read/write pathways
	int tmp = readID;
	readID = writeID;
	writeID = tmp;
	for (int i = 0; i < Constraint::solver_iterations; i++) {
		DistanceConstraintCompute->use();
		glFinish();
		glBindImageTexture(0, attachID[2*readID], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, DistanceTexID1, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32I);
		glBindImageTexture(2, DistanceTexID2, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32I);
		glBindImageTexture(3, DeltaTexXID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
		glBindImageTexture(4, DeltaTexYID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
		glBindImageTexture(5, DeltaTexZID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
		glBindImageTexture(6, RestDistanceTexID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
		glDispatchCompute(DistanceConstraintIndexData1.size(), 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glFinish();
		glCheckError();

		SuccessiveOverRelaxationCompute->use();
		//获得球的位置
		glUniform3fv(glGetUniformLocation(SuccessiveOverRelaxationCompute->ID, "sphere_pos"), 2 ,&scene->spherePos[0][0]);
		glUniform1fv(glGetUniformLocation(SuccessiveOverRelaxationCompute->ID, "radius"), 2, &scene->radius[0]);
		glFinish();
		glBindImageTexture(0, DeltaTexXID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
		glBindImageTexture(1, DeltaTexYID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
		glBindImageTexture(2, DeltaTexZID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
		glBindImageTexture(3, attachID[2* writeID], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(4, NiTexID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32I);
		glDispatchCompute(num_particles_width, num_particles_height, 1);

		//清空DeltaTexXID缓存里面的数据
		glClearTexImage(DeltaTexXID, 0, GL_RED_INTEGER, GL_INT, &Null_X[0]);
		glClearTexImage(DeltaTexYID, 0, GL_RED_INTEGER, GL_INT, &Null_X[0]);
		glClearTexImage(DeltaTexZID, 0, GL_RED_INTEGER, GL_INT, &Null_X[0]);
		
		//swap read/write pathways
		int tmp = readID;
		readID = writeID;
		writeID = tmp;
	}
	NormalCalcShader->use();
	for (int i = 0; i < 3; i++)
		glClearTexImage(NormalTexID[i], 0, GL_RED_INTEGER, GL_INT, &Null_X[0]);
	glFinish();
	glBindImageTexture(0, attachID[2 * readID], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, NormalTexID[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
	glBindImageTexture(2, NormalTexID[1], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
	glBindImageTexture(3, NormalTexID[2], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32I);
	glDispatchCompute(num_particles_width-1, num_particles_height-1, 1);
	glFinish();

	glBindBuffer(GL_PIXEL_PACK_BUFFER, vboID);
	glBindTexture(GL_TEXTURE_2D, attachID[2 * readID]);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, 0);

	readNormal();
	
	//重置状态
	//glReadBuffer(GL_NONE);
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glReadBuffer(GL_BACK);
	//glDrawBuffer(GL_BACK);

	glViewport(0, 0, scene->SCR_WIDTH, scene->SCR_HEIGHT);
	glEnable(GL_DEPTH_TEST);
}

/**
 * Sets up a floating point texture with the NEAREST filtering.
 */
void setupTexture(const GLuint texID, float *data,int width,int height) {
	// make active and bind
	glBindTexture(GL_TEXTURE_2D, texID);
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB, 16, 16);
	// turn off filtering and wrap modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// define texture with floating point format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0,
		GL_RGBA, GL_FLOAT, data);

}

/**
 * Sets up a floating point texture with the NEAREST filtering.
 */
void setupIntTexture(const GLuint texID, int *data, int width, int height) {
	// make active and bind
	glBindTexture(GL_TEXTURE_2D, texID);
	// turn off filtering and wrap modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0,
		GL_RED_INTEGER, GL_INT, data);

}

ComputeShaderCloth::ComputeShaderCloth(float _width, float _height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
{
	width = _width;
	height = _height;
	glfwSwapInterval(0);
	X.resize(total_points);
	X_last.resize(total_points);
	Normal.resize(total_points);
	TexCoord.resize(total_points);
	Ni.resize(total_points,0);
	NormalX.resize(total_points, 0);
	NormalY.resize(total_points, 0);
	NormalZ.resize(total_points, 0);
	Null_X.resize(total_points,0);
	// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
	for (int y = 0; y < num_particles_height; y++)
	{
		for (int x = 0; x < num_particles_width; x++)
		{
			vec3 pos = glm::vec3(((float(x) / (num_particles_width - 1)) * 2 - 1)* width / 2, 0, ((float(y) / (num_particles_height - 1))* height));

			X[(y*num_particles_width + x)] = vec4(pos, 1);
			X_last[(y*num_particles_width + x)] = vec4(pos, 1);
			Normal[(y*num_particles_width + x)] = vec4(0, 0, 0, 1);
			//纹理坐标，一块纹理覆盖一块布料
			vec2 temp;
			/*temp.x = x / (num_particles_width-1.0f);
			temp.y = y / (num_particles_width-1.0f);*/
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

	//填充约束数据
	for (int x = 0; x < num_particles_width; x++)
	{
		for (int y = 0; y < num_particles_height; y++)
		{
			// Distance Constraints
			if (x + 1 < num_particles_width) {
				DistanceConstraintIndexData1.push_back(i32vec2(x, y));
				Ni[(y*num_particles_width + x)]++;
				DistanceConstraintIndexData2.push_back(i32vec2(x+1, y));
				Ni[(y*num_particles_width + x + 1)]++;

				RestDistanceData.push_back(distance(X[y*num_particles_width + x], X[y*num_particles_width + x + 1]));
			}
				
			if (y + 1 < num_particles_height) {
				DistanceConstraintIndexData1.push_back(i32vec2(x, y));
				Ni[(y*num_particles_width + x)]++;
				DistanceConstraintIndexData2.push_back(i32vec2(x, y+1));
				Ni[((y+1)*num_particles_width + x)]++;

				RestDistanceData.push_back(distance(X[y*num_particles_width + x], X[(y+1)*num_particles_width + x]));
			}
			//Shear Springs
			if (y + 1 < num_particles_height && x + 1 < num_particles_width) {
				DistanceConstraintIndexData1.push_back(i32vec2(x, y));
				Ni[((y)*num_particles_width + x)]++;
				DistanceConstraintIndexData2.push_back(i32vec2(x+1, y + 1));
				Ni[((y+1)*num_particles_width + x+1)]++;

				RestDistanceData.push_back(distance(X[y*num_particles_width + x], X[(y + 1)*num_particles_width + x+1]));

				DistanceConstraintIndexData1.push_back(i32vec2(x+1, y));
				Ni[((y)*num_particles_width + x + 1)]++;
				DistanceConstraintIndexData2.push_back(i32vec2(x, y + 1));
				Ni[((y+1)*num_particles_width + x)]++;

				RestDistanceData.push_back(distance(X[y*num_particles_width + x+1], X[(y + 1)*num_particles_width + x]));
			}
		}
	}

	renderShader = ResourcesManager::loadShader("GPU_renderShader", "render.vs", "render.fs");
	DistanceConstraintCompute = ResourcesManager::loadComputeShader("DistanceConstraint", "DistanceConstraint.fs");
	SuccessiveOverRelaxationCompute = ResourcesManager::loadComputeShader("SOR", "SOR.fs");
	IntegrationShader = ResourcesManager::loadComputeShader("IntegrationShader", "Integration.fs");
	NormalCalcShader = ResourcesManager::loadComputeShader("NormalCalcShader", "NormalCalculate.fs");

	glCheckError();
	IntegrationShader->use();
	IntegrationShader->setFloat("global_dampening", global_dampening);
	IntegrationShader->setFloat("mass", 1.0f/total_points);
	IntegrationShader->setVec3("gravity", gravity);
	IntegrationShader->setFloat("dt", 1.0f / 50.0f);
	IntegrationShader->setInt("width", (num_particles_width));

	DistanceConstraintCompute->use();
	DistanceConstraintCompute->setFloat("wi", total_points);
	float k_prime = 1.0f - pow((1.0f - kStretch), 1.0f / Constraint::solver_iterations);
	DistanceConstraintCompute->setFloat("k_prime", k_prime);

	SuccessiveOverRelaxationCompute->use();
	SuccessiveOverRelaxationCompute->setInt("width", (num_particles_width));
	SuccessiveOverRelaxationCompute->setFloat("w", 1.0f);

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

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	_data[0] = &X[0].x;
	_data[1] = &X_last[0].x;

	// framebuffer configuration
	// -------------------------
	glGenFramebuffers(2, fboID);
	glGenTextures(4, attachID);
	for (int j = 0; j < 2; j++) {//两个帧缓冲，用于输入和输出交替
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID[j]);
		for (int i = 0; i < 2; i++) {//两块纹理，用于verlet积分的当前位置和过去位置
			setupTexture(attachID[i+2*j], _data[i], num_particles_width, num_particles_height);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, mrt[i], GL_TEXTURE_2D, attachID[i + 2 * j], 0);
		}
	}
	

	glGenTextures(1, &DistanceTexID1);
	glGenTextures(1, &DistanceTexID2);
	
	//DistanceConstraint纹理存储在默认帧缓冲中?
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, DistanceTexID1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32I, DistanceConstraintIndexData1.size(), 1, 0,
		GL_RG_INTEGER, GL_INT, &DistanceConstraintIndexData1[0].x);
	glCheckError();

	glBindTexture(GL_TEXTURE_2D, DistanceTexID2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32I, DistanceConstraintIndexData2.size(), 1, 0,
		GL_RG_INTEGER, GL_INT, &DistanceConstraintIndexData2[0].x);
	//存储XYZ三个方向的位置偏差，设为int是为了支持原子操作
	glGenTextures(1, &DeltaTexXID);
	setupIntTexture(DeltaTexXID, &Null_X[0], num_particles_width, num_particles_height);

	glGenTextures(1, &DeltaTexYID);
	setupIntTexture(DeltaTexYID, &Null_X[0], num_particles_width, num_particles_height);

	glGenTextures(1, &DeltaTexZID);
	setupIntTexture(DeltaTexZID, &Null_X[0], num_particles_width, num_particles_height);


	glGenTextures(1, &NiTexID);
	setupIntTexture(NiTexID, &Ni[0], num_particles_width, num_particles_height);

	glGenTextures(1, &RestDistanceTexID);
	glBindTexture(GL_TEXTURE_2D, RestDistanceTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, RestDistanceData.size(), 1, 0,
		GL_RED, GL_FLOAT, &RestDistanceData[0]);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID[0]);
	glGenTextures(3, NormalTexID);
	for (int i = 0; i < 3; i++) {
		setupIntTexture(NormalTexID[i], &Null_X[0], num_particles_width, num_particles_height);//attachID里面存放顶点数据
	}
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, NormalTexID[0], 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, NormalTexID[1], 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, NormalTexID[2], 0);
	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status == GL_FRAMEBUFFER_COMPLETE) {
		printf("FBO setup succeeded.");
	}
	else {
		printf("Problem with FBO setup.");
	}
	glCheckError();
}

void ComputeShaderCloth::calcNormal() {
	//刷新纹理数据
	for (int i=0;i<Normal.size();i++)
	{
		//Normal[i] = vec4(0);
		NormalX[i] = 0;
		NormalY[i] = 0;
		NormalZ[i] = 0;
	}
	//create smooth per particle normals by adding up all the (hard) triangle normals that each particle is part of
	for (int x = 0; x < num_particles_width - 1; x++)
	{
		for (int y = 0; y < num_particles_height - 1; y++)
		{
			//更新第一块三角形的法线
			vec4 p1 = X[y*num_particles_width + x + 1];
			vec4 p2 = X[y*num_particles_width + x];
			vec4 p3 = X[(y+1)*num_particles_width + x];
			vec3 v1 = p2 - p1;
			vec3 v2 = p3 - p1;
			vec3 normal = cross(v1, v2);

			NormalX[y*num_particles_width + x + 1] += normal.x*1000.0f;
			NormalY[y*num_particles_width + x + 1] += normal.y*1000.0f;
			NormalZ[y*num_particles_width + x + 1] += normal.z*1000.0f;

			NormalX[y*num_particles_width + x] += normal.x*1000.0f;
			NormalY[y*num_particles_width + x] += normal.y*1000.0f;
			NormalZ[y*num_particles_width + x] += normal.z*1000.0f;

			NormalX[(y + 1)*num_particles_width + x + 1] += normal.x*1000.0f;
			NormalY[(y + 1)*num_particles_width + x + 1] += normal.y*1000.0f;
			NormalZ[(y + 1)*num_particles_width + x + 1] += normal.z*1000.0f;

			//更新第二块三角形的法线
			p1 = X[(y+1)*num_particles_width + x + 1];
			p2 = X[y*num_particles_width + x+1];
			p3 = X[(y + 1)*num_particles_width + x];
			v1 = p2 - p1;
			v2 = p3 - p1;
			normal = cross(v1, v2);

			NormalX[(y + 1)*num_particles_width + x + 1] += normal.x*1000.0f;
			NormalY[(y + 1)*num_particles_width + x + 1] += normal.y*1000.0f;
			NormalZ[(y + 1)*num_particles_width + x + 1] += normal.z*1000.0f;

			NormalX[y*num_particles_width + x + 1] += normal.x*1000.0f;
			NormalY[y*num_particles_width + x + 1] += normal.y*1000.0f;
			NormalZ[y*num_particles_width + x + 1] += normal.z*1000.0f;

			NormalX[(y+1)*num_particles_width + x] += normal.x*1000.0f;
			NormalY[(y + 1)*num_particles_width + x] += normal.y*1000.0f;
			NormalZ[(y + 1)*num_particles_width + x] += normal.z*1000.0f;
		}
	}
}
void ComputeShaderCloth::render()
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

	//calcNormal();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(vaoID);

	//glBindBuffer(GL_ARRAY_BUFFER, NormalVboID[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles_width * num_particles_height * 1 * sizeof(int), &NormalX[0]);
	//glBindBuffer(GL_ARRAY_BUFFER, NormalVboID[1]);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles_width * num_particles_height * 1 * sizeof(int), &NormalY[0]);
	//glBindBuffer(GL_ARRAY_BUFFER, NormalVboID[2]);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, num_particles_width * num_particles_height * 1 * sizeof(int), &NormalZ[0]);

	glDrawElements(GL_TRIANGLES, 6 * (num_particles_height - 1)*(num_particles_width - 1), GL_UNSIGNED_INT, 0);
}
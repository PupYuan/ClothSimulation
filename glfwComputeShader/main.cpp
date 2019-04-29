#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include "utils/CReader.h"
#include <learnopengl\shader.h>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
#define WIDTH    16    //data block width
#define HEIGHT    16    //data block height

using namespace std;

void initGLSL(GLenum type);

void initFBO(unsigned unWidth, unsigned unHeight);

void initGLUT(int argc, char **argv);

void createTextures(void);

void setupTexture(const GLuint texID);

void performCompute(const GLuint inputTexID, const GLuint outputTexID);

void transferFromTexture(float *data);

void transferToTexture(float *data, GLuint texID);

// 纹理标识
GLuint outputTexID;
GLuint intermediateTexID;
GLuint inputTexID;

// GLSL 变量
GLuint glslProgram;
GLuint fragmentShader;

GLenum type = GL_FLOAT;

// FBO 标识
GLuint fb;

// 提供GL环境
GLuint glutWindowHandle;

Shader * computeShader;

struct structTextureParameters {
	GLenum texTarget;
	GLenum texInternalFormat;
	GLenum texFormat;
	char *shader_source;
} textureParameters;

float *pfInput;            //输入数据
unsigned unWidth = (unsigned)WIDTH;
unsigned unHeight = (unsigned)HEIGHT;
unsigned unSize = unWidth * unHeight;

GLfloat v[500];

// settings
const unsigned int SCR_WIDTH = 250;
const unsigned int SCR_HEIGHT = 100;

int main(int argc, char **argv) {
	//glfw+glad
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "glfwComputeShader", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	int i;
	// create test data
	unsigned unNoData = 4 * unSize;        //total number of Data
	pfInput = new float[unNoData];
	float *pfOutput = new float[unNoData];
	for (i = 0; i < unNoData; i++) pfInput[i] = 0.555;
	for (i = 0; i < 500; i++) {
		v[i] = i;
	}

	// create variables for GL
	textureParameters.texTarget = GL_TEXTURE_2D;
	textureParameters.texInternalFormat = GL_RGBA32F;
	textureParameters.texFormat = GL_RGBA;
	CReader reader;

	computeShader = new Shader("convolution.cs");
	// 初始化FBO
	initFBO(unWidth, unHeight);
	createTextures();

	/*char c_convolution[] = "convolution.cs";
	textureParameters.shader_source = reader.textFileRead(c_convolution);
	initGLSL(GL_COMPUTE_SHADER);*/
	//performCompute(inputTexID, intermediateTexID);

	//performCompute(intermediateTexID, outputTexID);
	performCompute(inputTexID, outputTexID);

	// get GPU results
	transferFromTexture(pfOutput);

	for (int i = 0; i < unNoData; i++) {
		cout << "input:" << pfInput[i] << " output:" << pfOutput[i] << endl;
	}

	// clean up
	glDetachShader(glslProgram, fragmentShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(glslProgram);
	glDeleteTextures(1, &inputTexID);
	glDeleteTextures(1, &outputTexID);

	// exit
	delete pfInput;
	delete pfOutput;
	return EXIT_SUCCESS;
}

/**
 * Off-screen Rendering.
 */
void initFBO(unsigned unWidth, unsigned unHeight) {
	// create FBO (off-screen framebuffer)
	glGenFramebuffers(1, &fb);
	// bind offscreen framebuffer (that is, skip the window-specific render target)
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

}

/**
 * Set up the GLSL runtime and creates shader.
 */
void initGLSL(GLenum type) {
	// create program object
	glslProgram = glCreateProgram();
	// create shader object (fragment shader)
	fragmentShader = glCreateShader(type);
	// set source for shader
	const GLchar *source = textureParameters.shader_source;
	glShaderSource(fragmentShader, 1, &source, nullptr);
	// compile shader
	glCompileShader(fragmentShader);

	// attach shader to program
	glAttachShader(glslProgram, fragmentShader);
	// link into full program, use fixed function vertex shader.
	// you can also link a pass-through vertex shader.
	glLinkProgram(glslProgram);

}

/**
 * create textures and set proper viewport etc.
 */
void createTextures() {
	// 创建两个纹理
	// y 输出; x 输入.
	glGenTextures(1, &outputTexID);
	glGenTextures(1, &intermediateTexID);
	glGenTextures(1, &inputTexID);
	// set up textures
	setupTexture(outputTexID);
	setupTexture(intermediateTexID);
	setupTexture(inputTexID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureParameters.texTarget, inputTexID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, textureParameters.texTarget, intermediateTexID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, textureParameters.texTarget, outputTexID, 0);
	transferToTexture(pfInput, inputTexID);
	// set texenv mode
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

/**
 * Sets up a floating point texture with the NEAREST filtering.
 */
void setupTexture(const GLuint texID) {
	// make active and bind
	glBindTexture(textureParameters.texTarget, texID);
	glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA32F, 16, 16);
	// turn off filtering and wrap modes
	glTexParameteri(textureParameters.texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(textureParameters.texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(textureParameters.texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(textureParameters.texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// define texture with floating point format
	glTexImage2D(textureParameters.texTarget, 0, textureParameters.texInternalFormat, unWidth, unHeight, 0,
		textureParameters.texFormat, type, nullptr);
}

void performCompute(const GLuint inputTexID, const GLuint outputTexID) {

	computeShader->use();
	// enable GLSL program
	//glUseProgram(glslProgram);
	// enable the read-only texture x
	//glActiveTexture(GL_TEXTURE0);
	//glUniform1fv(glGetUniformLocation(glslProgram, "v"), 4, v);

	// Synchronize for the timing reason.
	glFinish();

	//CTimer timer;
	//long lTime;
	//timer.reset();

	glBindImageTexture(0, inputTexID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, outputTexID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glFinish();
	//lTime = timer.getTime();
	//cout << "Time elapsed: " << lTime << " ms." << endl;
}

/**
 * Transfers data from currently texture to host memory.
 */
void transferFromTexture(float *data) {
	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glReadPixels(0, 0, unWidth, unHeight, textureParameters.texFormat, type, data);
}

/**
 * Transfers data to texture. Notice the difference between ATI and NVIDIA.
 */
void transferToTexture(float *data, GLuint texID) {
	// 绑定 为帧缓存， 以后的着色器操作均会在此纹理上进行
	glBindTexture(textureParameters.texTarget, texID);
	glTexSubImage2D(textureParameters.texTarget, 0, 0, 0, unWidth, unHeight, textureParameters.texFormat, type, data);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
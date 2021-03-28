//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::vec3 cameraDirection(0.0f, 0.0f, -1.0f);
gps::Camera myCamera(
	glm::vec3(0.0f, 2.0f, 5.5f),
	cameraDirection,
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.1f;

bool pressedKeys[1024];
GLfloat angleY = 0.0f;
GLfloat angle;
GLfloat lightAngle;

gps::Model3D lightCube;
gps::Model3D scene;
gps::Model3D car;
gps::Model3D swingChair;
gps::Model3D swingFrame;
gps::Model3D snowflake;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

float ok = 1.0f;
float lightStatus = 1.0f;
float snowflakes = 0.0f;
bool initialCoordBool = true;
float initialX = glWindowWidth / 2.0;
float initialY = glWindowHeight / 2.0;
float limitX = -90.0f;
float limitY = 0.0f;
float carCoords = 0.0f;
float snowflakeInc = 0.0f;
bool snow = true;

float startingX, startingY, startingZ = 0.0f;
float delta = 0.6f;

float swingCoords = 0.0f;
float swingRotation = 0.0f;
bool swingVerif = true;

bool presentationAnimation = false;

float transparency = 1.0f;
float transp = 0.0f;
bool transpMain = true;

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		if (ok > 0.5f) {
			ok = 0.0f;
		}
		else {
			ok = 1.0f;
		}
	}

	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		if (lightStatus > 0.5f) {
			lightStatus = 0.0f;
		}
		else {
			lightStatus = 1.0f;
		}
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		if (snowflakes < 0.5f) {
			snowflakes = 1.0f;
		}
		else {
			snowflakes = 0.0f;
		}
	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		if (presentationAnimation) {
			presentationAnimation = false;
		}
		else {
			presentationAnimation = true;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (initialCoordBool) {
		initialX = xpos;
		initialY = ypos;
		initialCoordBool = false;
	}

	float x = xpos - initialX;
	float y = initialY - ypos;
	initialX = xpos;
	initialY = ypos;

	float sensitivity = 0.1f;
	x *= sensitivity;
	y *= sensitivity;

	
	limitX += x;
	limitY += y;

	//if (abs(limitY) > 89.0f)
	//	limitY = copysign(89.0f, limitY);
	if (limitY > 89.0f)
		limitY = 89.0f;
	if (limitY < -89.0f)
		limitY = -89.0f;

	myCamera.rotate(limitY, limitX);
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		angle -= 2.0f;
		myCamera.rotate(1.0f, angle);
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle += 2.0f;
		myCamera.rotate(1.0f, angle);
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_DOWN]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_LEFT] && carCoords < 1.5f) {
		carCoords += 0.1f;
	}

	if (pressedKeys[GLFW_KEY_RIGHT] && carCoords > -1.0f) {
		carCoords -= 0.1f;
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	//glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	glEnable(GL_BLEND);

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	lightCube.LoadModel("objects/cube/cube.obj");
	scene.LoadModel("objects/scene/scene.obj");
	car.LoadModel("objects/car/car.obj");
	swingChair.LoadModel("objects/swing/swingChair.obj");
	swingFrame.LoadModel("objects/swing/swingFrame.obj");
	snowflake.LoadModel("objects/Snowflake/fulgi.obj");
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();

	depthMapShader.loadShader("shaders/lightVertex.vert", "shaders/lightVertex.frag");
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initFBO() {

	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);


	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


glm::mat4 computeLightSpaceTrMatrix() {
	glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();

	float counter = glfwGetTime();

	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "ok"), ok);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "lightStatus"), lightStatus);

	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	scene.Draw(shader);

	if (counter > 140.0f) {
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}

		swingFrame.Draw(shader);

		if (swingCoords < 0.02f && swingVerif) {
			swingCoords += 0.001f;
			swingRotation += 0.005f;
			if (swingCoords > 0.019f)
				swingVerif = false;
		}
		else if (swingCoords > -0.02f && !swingVerif) {
			swingCoords -= 0.001f;
			swingRotation -= 0.005f;
			if (swingCoords < -0.019f)
				swingVerif = true;
		}
		model = glm::translate(model, glm::vec3(swingCoords, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(swingRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}

		swingChair.Draw(shader);
	}
	if (snowflakes > 0.5f) {
		if (snowflakeInc < 1.0f) {
			snowflakeInc += 0.2f;
		}
		else {
			snowflakeInc = 0.0f;
		}
		model = glm::translate(model, glm::vec3(0.0f, snowflakeInc, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// do not send the normal matrix if we are rendering in the depth map
		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}

		snowflake.Draw(shader);
	}

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, carCoords));
	if (transpMain && transparency > 0.0f) {
		transparency -= 0.1f;
		if (transparency < 0.1f)
			transpMain = false;
	}
	else if (transpMain == false && transparency < 1.0f) {
		transparency += 0.1f;
		if (transparency > 0.9f)
			transpMain = true;
	}
	transp = 1.0f;
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "transp"), transp);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "transparency"), transparency);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	car.Draw(shader);
	transp = 0.0f;
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "transp"), transp);
}

void renderScene() {



	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)
		if (presentationAnimation) {
			int a = 0;
			while (a < 35 && startingX < 360.0f) {

				myCamera.move(gps::MOVE_RIGHT, 0.07f);
				startingX = startingX + 0.35f;
				startingY = 8.0f;

				float camX = delta * -sinf(startingX * (3.0f / 180.0f)) * cosf((startingY) * (3.0f / 180.0f));
				float camY = delta * -sinf((startingY) * (3.0f / 180.0f));
				float camZ = -delta * cosf((startingX) * (3.0f / 180.0f)) * cosf((startingY) * (3.0f / 180.0f));

				glm::vec3 front;
				front.x = camX;
				front.y = camY;
				front.z = camZ;
				myCamera.setCameraDirection(front);
				a++;

			}

		}
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		//lightDir
		model = glm::translate(model, 1.0f * glm::vec3(0.0f, 5.0f, 4.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);
	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char* argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();

	glCheckError();

	GLuint shadowMapFBO;
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);


	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	cleanup();

	return 0;
}
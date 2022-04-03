﻿#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Lamp.h"
#include "Skeleton.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void wireframeMode(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

vector<glm::mat4> Transforms;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float animationTime = 0.0f;

int main(void)
{
	GLFWwindow* window;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Master 2018", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);

	Shader modelShader("res/shaders/model_vs.hlsl", "res/shaders/model_fs.hlsl");
	Shader lampShader("res/shaders/lamp.vs", "res/shaders/lamp.fs");
	Shader skeletonShader("res/shaders/skeleton.vs", "res/shaders/skeleton.fs");

	//Model aModel("res/object/body/pedobear_animated.fbx");
	//Model aModel("res/object/body/skinning_test_2.fbx");
	//Model aModel("res/object/body/skinning_test.fbx");
	//Model aModel("res/object/body/skinning_test_3.fbx");
	//Model aModel("res/object/body/silly_dance.fbx");
	//Model aModel("res/object/body/Mannequin_Animation.fbx");
	//Model aModel("res/object/body/turtle_texture.fbx");
	Model aModel("res/object/cylinder/leafbone.fbx");
	//Model aModel("res/object/body/groo.fbx");
	//Model aModel("res/object/body/sk2_leafbone.fbx");

	//				lamp position					light color
	Lamp lamp(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f));


	float startFrame = glfwGetTime();
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		animationTime = currentFrame - startFrame;

		//Input...
		processInput(window);
		wireframeMode(window);

		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//wireframe mode for debugging
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//activate model shader
		// render 3D model
		modelShader.use(); //3d model shader
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		modelShader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		modelShader.setMat4("view", view);
		glm::mat4 model= glm::mat4(1.0f);
		//model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));	// it's a bit too big for our scene, so scale it down
		modelShader.setMat4("model", model);
		/*aModel.BoneTransform(animationTime, Transforms);
		for (unsigned int i = 0; i < Transforms.size(); ++i)
		{
			const std::string name = "gBones[" + std::to_string(i) + "]";
			GLuint boneTransform = glGetUniformLocation(modelShader.ID, name.c_str());
			glUniformMatrix4fv(boneTransform, 1, GL_FALSE, glm::value_ptr(Transforms[i]));
		}*/
		//set uniforms for model shader
		modelShader.setVec3("lightPos", lamp.Position);
		modelShader.setVec3("lightColor", lamp.Color);
		modelShader.setVec3("viewPos", camera.Position);

		aModel.Draw(modelShader);

		//activate lamp shader
		//render light cube(lamp)
		//lamp.Position.x = 1.0f + sin(glfwGetTime()) * 2.0f;
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		glm::mat4 lamp_cube;
		//lamp_cube = glm::rotate(lamp_cube, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
		lamp_cube = glm::translate(lamp_cube, lamp.Position);
		lamp_cube = glm::scale(lamp_cube, glm::vec3(0.2f));	// it's a bit too big for our scene, so scale it down
		//set uniforms for lamp shader
		lampShader.setMat4("model", lamp_cube);
		lamp.Draw(lampShader);

		//activate skeleton shader
		//Skeleton skeleton(aModel.skeleton_pose);

		//skeletonShader.use();
		//skeletonShader.setMat4("projection", projection);
		//skeletonShader.setMat4("view", view);
		//glm::mat4 skeletom_model;
		//skeletom_model = glm::scale(skeletom_model, glm::vec3(0.005f, 0.005f, 0.005f));	// it's a bit too big for our scene, so scale it down
		//skeletonShader.setMat4("model", skeletom_model);
		//skeleton.Draw(skeletonShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void wireframeMode(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}



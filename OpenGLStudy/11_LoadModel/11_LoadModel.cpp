
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Camera.h>
#include <Shader.h>
#include <Mesh.h>
#include <Model.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera = Camera(glm::vec3(0, 2.0f, 3.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float deltaTime;
float lastFrame;

bool showConvolutionMap = true;
bool spaceKeyPressed = false;


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	float vertices[] = {
		// positions         
		-0.5f, -0.5f, -0.5f, 
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// 只需要绑定VBO不用再次设置VBO的数据，因为箱子的VBO数据中已经包含了正确的立方体顶点数据
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 设置灯立方体的顶点属性（对我们的灯来说仅仅只有位置数据）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.0f,  2.0f,  1.5f),
		glm::vec3(0.0f,  3.5f,  0.0f),
		glm::vec3(0.0f,  2.0f,  -1.5f),
		glm::vec3(0.5f,  3.0f,  0.5f)
	};
	
	// build and compile our shader program
	// ------------------------------------
	Shader modelShader("model_shader.vs", "model_shader.fs");
	Shader lightShader("light_shader.vs", "light_shader.fs");

	Model nanosuitModel = Model("./Model/nanosuit/nanosuit.obj");

	glEnable(GL_DEPTH_TEST);

	modelShader.use();

	modelShader.setVec3("dirLight.direction", 1.0f, 0.0f, 0.0f);
	modelShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
	modelShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f); // 将光照调暗了一些以搭配场景
	modelShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

	modelShader.setFloat("spotLight.constant", 1.0f);
	modelShader.setFloat("spotLight.linear", 0.09f);
	modelShader.setFloat("spotLight.quadratic", 0.032f);
	modelShader.setFloat("spotLight.cutoff_inner", glm::cos(glm::radians(12.5f)));
	modelShader.setFloat("spotLight.cutoff_outter", glm::cos(glm::radians(17.5f)));
	modelShader.setVec3("spotLight.ambient", 0.4f, 0.4f, 0.4f);
	modelShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f); // 将光照调暗了一些以搭配场景
	modelShader.setVec3("spotLight.specular", 2.0f, 2.0f, 2.0f);

	modelShader.setVec3("pointLights[0].position", pointLightPositions[0]);
	modelShader.setFloat("pointLights[0].constant", 1.0f);
	modelShader.setFloat("pointLights[0].linear", 0.22f);
	modelShader.setFloat("pointLights[0].quadratic", 0.2f);
	modelShader.setVec3("pointLights[0].ambient", 0.2f, 0.2f, 0.2f);
	modelShader.setVec3("pointLights[0].diffuse", 0.5f, 0.5f, 0.5f); // 将光照调暗了一些以搭配场景
	modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

	modelShader.setVec3("pointLights[1].position", pointLightPositions[1]);
	modelShader.setFloat("pointLights[1].constant", 1.0f);
	modelShader.setFloat("pointLights[1].linear", 0.22f);
	modelShader.setFloat("pointLights[1].quadratic", 0.2f);
	modelShader.setVec3("pointLights[1].ambient", 0.2f, 0.2f, 0.2f);
	modelShader.setVec3("pointLights[1].diffuse", 0.5f, 0.5f, 0.5f); // 将光照调暗了一些以搭配场景
	modelShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);

	modelShader.setVec3("pointLights[2].position", pointLightPositions[2]);
	modelShader.setFloat("pointLights[2].constant", 1.0f);
	modelShader.setFloat("pointLights[2].linear", 0.22f);
	modelShader.setFloat("pointLights[2].quadratic", 0.2f);
	modelShader.setVec3("pointLights[2].ambient", 0.2f, 0.2f, 0.2f);
	modelShader.setVec3("pointLights[2].diffuse", 0.5f, 0.5f, 0.5f); // 将光照调暗了一些以搭配场景
	modelShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);

	modelShader.setVec3("pointLights[3].position", pointLightPositions[3]);
	modelShader.setFloat("pointLights[3].constant", 1.0f);
	modelShader.setFloat("pointLights[3].linear", 0.22f);
	modelShader.setFloat("pointLights[3].quadratic", 0.2f);
	modelShader.setVec3("pointLights[3].ambient", 0.2f, 0.2f, 0.2f);
	modelShader.setVec3("pointLights[3].diffuse", 0.5f, 0.5f, 0.5f); // 将光照调暗了一些以搭配场景
	modelShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);

	lightShader.use();
	lightShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		modelShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		modelShader.setMat4("view", view);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		modelShader.setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		modelShader.setMat4("model", model);

		modelShader.setVec3("viewPos", camera.Position);
		modelShader.setVec3("spotLight.position", camera.Position);
		modelShader.setVec3("spotLight.direction", camera.Front);

		modelShader.setBool("useNormalMapping", showConvolutionMap);

		nanosuitModel.Draw(modelShader);

		{
			glBindVertexArray(lightVAO);
			lightShader.use();
			glm::mat4 view = camera.GetViewMatrix();
			lightShader.setMat4("view", view);
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			lightShader.setMat4("projection", projection);
			for (int i = 0; i < 4; i++) {
				glm::mat4 model = glm::mat4(1.0);
				float radius = 3.0f;
				//glm::vec3 lightPosition = glm::vec3(cos(lastFrame)*radius, 0.2f, sin(lastFrame)*radius);
				glm::vec3 lightPosition = pointLightPositions[i];
				model = glm::translate(model, lightPosition);
				model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
				lightShader.setMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
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
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spaceKeyPressed)
	{
		showConvolutionMap = !showConvolutionMap;
		spaceKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		spaceKeyPressed = false;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

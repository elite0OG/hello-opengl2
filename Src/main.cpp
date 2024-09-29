#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include"stb_image.h"
struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource passshader(const std::string& filepath) {
	enum shaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream stream(filepath);
	std::stringstream ss[2];
	std::string line;
	shaderType type = shaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = shaderType::VERTEX;  // Assignment corrected here
			}
			else if (line.find("fragment") != std::string::npos) {
				type = shaderType::FRAGMENT;  // Assignment corrected here
			}
		}
		else {
			ss[(int)type] << line << '\n';  // This should be outside the if block
		}
	}

	return { ss[0].str(), ss[1].str() };
}

int  success;
char infoLog[512];
void call_back(GLFWwindow* window, int h, int w) {
	glViewport(0, 0, w, h);
}
void processinput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS) {
		glfwSetWindowShouldClose(window,true);
	}
	
}
void checkCompilation(unsigned int  Shader) {
	 
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(Shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}
 

void checkLinking(unsigned int Program) {
	glGetProgramiv(Program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
}
int main() {
	//initionlision glfw
	glfwInit();
	// giving vertion to opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// giving minor vertion to open gl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//making window to in
	 glfwWindowHint(GLFW_RESIZABLE, 0);
	  // chohsing core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//createing window

	GLFWwindow* window = glfwCreateWindow(800, 600, "starlay-project v0.0.10", NULL, NULL);
	//checking it is it working or ont window
	if (window == NULL) {
		std::cout << "hello i think you messed up noob" << std::endl;
		glfwTerminate();
		return -1;
	}
	// seting the vewport form function
	glfwSetFramebufferSizeCallback(window, call_back);
	// seting context 
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE; // Allow modern OpenGL functions
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	// objects to work
	unsigned int VAO,VBO,EBO;
	// generating vertex arrays called VAO
	glGenVertexArrays(1, &VAO);
	// generating buffer called VBO
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//binding VAO
	glBindVertexArray(VAO);
	//BINDING VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//providing data to buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// giving data and location to the vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(1);
	// it says that if we want to use vertexattrib 
	glEnableVertexAttribArray(0);
	//binding buffer for vbo from 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Binding vertex arrays called vao
	glBindVertexArray(0);
	ShaderProgramSource source = passshader("Src/shaders/basic.shader");
	const char* vertexSource = source.VertexSource.c_str();
	const char* fragmentSource = source.FragmentSource.c_str();

	unsigned int vertexShaders;
	vertexShaders = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaders, 1, &vertexSource, NULL);
	 
	glCompileShader(vertexShaders);
	checkCompilation(vertexShaders);

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	checkCompilation(fragmentShader);

	// to use shaders in program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaders);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkLinking(shaderProgram);


	unsigned int texture;
	int heigth, width,numColch;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load("Src/shaders/wall.jpg",&width,&heigth,&numColch,0);
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	 
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint tex0unit = glGetUniformLocation(shaderProgram, "ourTexture");
	glUniform1i(tex0unit, 0);
	 
 
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		 
		glUseProgram(shaderProgram);
		// binding it again imp
		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, texture);
		// now drawing it 
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		// processing input
		processinput(window);
		//swaping back anf front buffer
		glfwSwapBuffers(window);
		// checing any event on srean
		glfwPollEvents();
	}
	//cleaning stuff
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &texture);
	
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShaders);
	glDeleteShader(fragmentShader);

	glfwTerminate();
	return 0;
}
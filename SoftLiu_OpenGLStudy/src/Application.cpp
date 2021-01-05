#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath)
{
	std::ifstream stream(filePath);
	// 判断是否打开
	if (!stream.is_open())
	{
		// 没有打开成功
		std::cout << "file open failed, file path:" << filePath << std::endl;
		return{};
	}

	enum  class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return{ ss[0].str(),ss[1].str() };
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		// 设置关闭window窗口
		//glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		std::cout << "Key Space Press." << std::endl;
	}
}

// 编译着色器
static unsigned int CompileShader(unsigned int type, const std::string &source)
{
	// 创建着色器对象
	unsigned int id = glCreateShader(type);
	// 将顶点着色源码 附加到着色器对象上
	const char *res = source.c_str();
	glShaderSource(id, 1, &res, NULL);
	// 编译着色器对象
	glCompileShader(id);
	// 判断着色器对象是否编译成功
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		// 在栈中申请一片内存，无须手动释放
		char *message = (char*)alloca(length * sizeof(char));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " Error: " << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

// 创建Shader程序对象
static unsigned int CreateShader(const std::string &vertexSource, const std::string fragmentSource)
{
	// 创建 程序对象
	unsigned int program = glCreateProgram();
	// 编译着色器源码
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	// 将着色器附加到程序对象上
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	// 链接着色器程序
	glLinkProgram(program);
	int result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char *message = (char*)alloca(length * sizeof(char));
		std::cout << "Failed to Create Shader Program " << " Error: " << std::endl;
		std::cout << message << std::endl;
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

int main(void)
{
	if (!glfwInit())
	{
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// 设置 窗口大小变化事件回调函数
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* curWindow, int curWidth, int curHeight) {
		// 视口 -- 设置窗口的维度
		glViewport(0, 0, curWidth, curHeight);
	});
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// 设置渲染窗口
	glViewport(0, 0, 800, 600);

	// 绘制三角形
	// 设置 坐标点
	float vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};
	// 声明 顶点数组对象
	unsigned int VAO;
	// 声明 顶点缓冲对象
	unsigned int VBO;
	// 创建 顶点数组对象
	glGenVertexArrays(1, &VAO);
	// 创建 顶点缓冲对象
	glGenBuffers(1, &VBO);
	// 声明索引缓冲对象
	unsigned int EBO;
	// 创建 索引缓冲对象
	glGenBuffers(1, &EBO);
	// 绑定 顶点数组对象
	glBindVertexArray(VAO);
	// 绑定 顶点缓冲对象
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 配置当前绑定的缓冲(VBO)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 绑定 索引缓冲对象
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 告诉OpenGL该如何解析顶点数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 已经将 顶点缓冲对象绑定到
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	// 创建 Shader程序

	// 顶点着色器小程序， 对应OpenGL版本 3.3
	const char* vertexShaderSource =
		"#version 330 core\n" // 对应使用的核心版本
		"layout (location = 0) in vec3 aPos;\n" // 声明输入顶点属性
		"void main()\n"
		"{\n"
		"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"// 内置变量 gl_Position
		"}\n";

	// 片段着色器 对应OpenGL版本 3.3
	const char* fragementShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"// 输出变量，最终输出的颜色
		"void main()\n"
		"{\n"
		"FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
		"}\n";
	ShaderProgramSource shaderProgramSource = ParseShader("res/shaders/Basic.shader");
	unsigned int program = CreateShader(shaderProgramSource.VertexSource, shaderProgramSource.FragmentSource);

	while (!glfwWindowShouldClose(window))
	{
		// 设置清屏背景颜色
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 输入函数
		processInput(window);

		glUseProgram(program);
		// 绘制三角形
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		// 绑定 顶点数组对象
		//glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(program);

	// 清除 OpenGL窗口资源
	glfwTerminate();
	std::cout << "OpenGL Window Closed." << std::endl;
	system("pause");
	return 0;
}
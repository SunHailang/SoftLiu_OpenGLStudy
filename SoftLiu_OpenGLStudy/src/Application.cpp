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
	// �ж��Ƿ��
	if (!stream.is_open())
	{
		// û�д򿪳ɹ�
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
		// ���ùر�window����
		//glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		std::cout << "Key Space Press." << std::endl;
	}
}

// ������ɫ��
static unsigned int CompileShader(unsigned int type, const std::string &source)
{
	// ������ɫ������
	unsigned int id = glCreateShader(type);
	// ��������ɫԴ�� ���ӵ���ɫ��������
	const char *res = source.c_str();
	glShaderSource(id, 1, &res, NULL);
	// ������ɫ������
	glCompileShader(id);
	// �ж���ɫ�������Ƿ����ɹ�
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		// ��ջ������һƬ�ڴ棬�����ֶ��ͷ�
		char *message = (char*)alloca(length * sizeof(char));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " Error: " << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

// ����Shader�������
static unsigned int CreateShader(const std::string &vertexSource, const std::string fragmentSource)
{
	// ���� �������
	unsigned int program = glCreateProgram();
	// ������ɫ��Դ��
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	// ����ɫ�����ӵ����������
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	// ������ɫ������
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
	// ���� ���ڴ�С�仯�¼��ص�����
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* curWindow, int curWidth, int curHeight) {
		// �ӿ� -- ���ô��ڵ�ά��
		glViewport(0, 0, curWidth, curHeight);
	});
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// ������Ⱦ����
	glViewport(0, 0, 800, 600);

	// ����������
	// ���� �����
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
	// ���� �����������
	unsigned int VAO;
	// ���� ���㻺�����
	unsigned int VBO;
	// ���� �����������
	glGenVertexArrays(1, &VAO);
	// ���� ���㻺�����
	glGenBuffers(1, &VBO);
	// ���������������
	unsigned int EBO;
	// ���� �����������
	glGenBuffers(1, &EBO);
	// �� �����������
	glBindVertexArray(VAO);
	// �� ���㻺�����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// ���õ�ǰ�󶨵Ļ���(VBO)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// �� �����������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// ����OpenGL����ν�����������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// �Ѿ��� ���㻺�����󶨵�
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	// ���� Shader����

	// ������ɫ��С���� ��ӦOpenGL�汾 3.3
	const char* vertexShaderSource =
		"#version 330 core\n" // ��Ӧʹ�õĺ��İ汾
		"layout (location = 0) in vec3 aPos;\n" // �������붥������
		"void main()\n"
		"{\n"
		"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"// ���ñ��� gl_Position
		"}\n";

	// Ƭ����ɫ�� ��ӦOpenGL�汾 3.3
	const char* fragementShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"// ��������������������ɫ
		"void main()\n"
		"{\n"
		"FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
		"}\n";
	ShaderProgramSource shaderProgramSource = ParseShader("res/shaders/Basic.shader");
	unsigned int program = CreateShader(shaderProgramSource.VertexSource, shaderProgramSource.FragmentSource);

	while (!glfwWindowShouldClose(window))
	{
		// ��������������ɫ
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ���뺯��
		processInput(window);

		glUseProgram(program);
		// ����������
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		// �� �����������
		//glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(program);

	// ��� OpenGL������Դ
	glfwTerminate();
	std::cout << "OpenGL Window Closed." << std::endl;
	system("pause");
	return 0;
}
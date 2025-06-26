#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

const std::string vertexShaderPath = "shaders/vertex-shader.glsl";
const std::string fragmentShaderPath = "shaders/fragment-shader.glsl";

static unsigned int compileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << std::format("Failed to compile {} shader", type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cerr << message << std::endl;

        return 0;
    }

    return id;
}

static int createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

static std::string readFile(std::string path)
{
    std::ifstream inputFile(path);
    if (!inputFile)
    {
        std::cerr << "Cannot read file: " << path << std::endl;
        return "";
    }
    std::ostringstream buffer;
    std::string line;
    while (std::getline(inputFile, line))
    {
        buffer << line << std::endl;
    }
    return buffer.str();
}

int main(void)
{
    GLFWwindow *window;

    if (!glfwInit()) return -1;

    window = glfwCreateWindow(800, 600, "OpenGL Project", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) std::cerr << "GLEW not OK!" << std::endl;

    const float positions[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        -0.5f, 0.5f,
        0.5f, 0.5f
    };

    const unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    unsigned int vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    unsigned int indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    unsigned int shader = createShader(readFile(vertexShaderPath), readFile(fragmentShaderPath));
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
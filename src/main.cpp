/*
  Credit to https://learnopengl.com/Getting-started/Hello-Window for the tutorial
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void update(GLFWwindow* window)
{
  /*
    glfwGetKey` returns 1 if pressed else 0
  */

  if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
}

void render()
{
  // Clear screen using clear color
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw Vertex Arrays
  /*
    frist argument: the type of primitive we would like to draw
    second argument: the index of the vertex array
    third argument: the number of vertices to draw
  */
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

std::string readFile(std::string path)
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

unsigned int compileShader(unsigned int type, std::string source)
{
  // Create a pointer to the first char of the shader string
  const char* src = source.c_str();

  // Create a shader object referenced by an id
  unsigned int shader = glCreateShader(type);

  // Set shader source and compile shader
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);

  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << std::endl;
    std::cerr << infoLog << std::endl;
    return 0;
  }

  // return shader id
  return shader;
}

unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
    // Create shader program
  /*
    A shader program links all the shaders together to help them communicate with each other
  */
  unsigned int shaderProgram = glCreateProgram();

  // Link shaders together
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // Delete the shader objects since we no longer need them
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader); 

  int success;
  char infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cerr << "Failed to Link Program" << std::endl;
    std::cerr << infoLog << std::endl;
    return 0;
  }

  return shaderProgram;
}

int main() {
  // Initialize GLFW
  glfwInit();
  
  // Tell GLFW we are using OpenGL 4.6 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  // Tell GLFW we are using OpenGL Core
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Initialise window + check for errors + make window current context
  GLFWwindow* window = glfwCreateWindow(800, 600,"Learn OpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cerr << "Failed to create GLFW window" << std::endl;
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initialise GLAD + error handling
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // Tell OpenGL the window size so thet we can use normalized device coordinates
  glViewport(0, 0, 800, 600);

  // Resize window if window size changes
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Set clear color
  glClearColor(.5f, .5f, .5f, 1);

  // Define vertices
  // Each vertex has 3 attributes : x, y, z
  float vertices[] = {
    -.5f, -.5f, 0,
    -.5f, .5f, 0,
    .5f, .5f, 0,
    .5f, -.5f, 0
  };

  unsigned int indices[] = {
    0, 3, 2,
    2, 1, 0
  };

  // Generate vertex buffer, vertex attribute array
  unsigned int vbo, vao, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  // Bind Vertex Array
  glBindVertexArray(vao);

  // Bind buffers to its buffer type
  /*
    OpenGL allows you to bind multiple buffers as long as they have separate buffer types
  */
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  // Copies the data to the buffer bound to the its respective buffer type
  /*
    first argument: type of the buffer we want to copy data into
    second argument: size of the data in bytes
    third argument: data
    fourth argument: specifies the graphics card how you want to manage the data
      - GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
      - GL_STATIC_DRAW: the data is set only once and used many times.
      - GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
  */
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Tells OpenGL the layout of out vertex data
  /*
    first argument: location of the vertex attribute array we want to configure
    second argument: number of values in the vertex attribute
    third argument: data type
    forth argument: whether the values should be normalized
    fifth argument: the space between the start of two consecutive vertex attributes in the array in bytes (also known as stride)
    sixth argument: offset of where the position data begins (it has to be cast into a void pointer for some reason, idk why)
  */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Compile and link shaders
  unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, readFile("shaders/vertex-shader.glsl"));
  unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, readFile("shaders/fragment-shader.glsl"));
  unsigned int shaderProgram = createProgram(vertexShader, fragmentShader);

  // Use the program we just created
  glUseProgram(shaderProgram);

  while(!glfwWindowShouldClose(window))
  {
    update(window);

    // rendering commands go here:
    render();

    // Swaps front and back buffers
    /*
      An application takes time to draw all the pixels on the screen
      When an application draws on a single buffer, it can have a flickering effect as not all pixels have been drawn when the screen renders
      To solve this issue, applications implement double buffers
      The front buffer contains the final output image shown on the screen
      All the rendering commands draw to the back buffer
      As soon as the back buffer is complete, they swap, instantaneously changing frames without flickering
    */
    glfwSwapBuffers(window);

    // Checks if any events are triggered (keyboard, mouse, etc)
    glfwPollEvents();
  }

  // Proper cleanup
  glfwTerminate();

  return 0;
}
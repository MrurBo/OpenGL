/**
 * Credit to https://learnopengl.com/Getting-started/Hello-Window for the tutorial
*/

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void update(GLFWwindow* window)
{
  /**
   * glfwGetKey` returns 1 if pressed else 0
  */

  if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
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
  /**
   * A shader program links all the shaders together to help them communicate with each other
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
  GLFWwindow* window = glfwCreateWindow(800, 800,"Learn OpenGL", NULL, NULL);
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
  glViewport(0, 0, 800, 800);

  // Resize window if window size changes
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Set clear color
  glClearColor(.5f, .5f, .5f, 1);

  // Define vertices
  // Each vertex has 5 attributes : x, y, z, u, v
  float vertices[] = {
    -.25f, -.25f, 0, 0, 0,
    -.25f, .25f, 0, 0, 1,
    .25f, .25f, 0, 1, 1,
    .25f, -.25f, 0, 1, 0
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
  /**
   * OpenGL allows you to bind multiple buffers as long as they have separate buffer types
  */
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  // Copies the data to the buffer bound to the its respective buffer type
  /**
   * first argument: type of the buffer we want to copy data into
   * second argument: size of the data in bytes
   * third argument: data
   * fourth argument: specifies the graphics card how you want to manage the data
   *   - GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
   *   - GL_STATIC_DRAW: the data is set only once and used many times.
   *   - GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
  */
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Tells OpenGL the layout of out vertex data
  /**
   * first argument: location of the vertex attribute array we want to configure
   * second argument: number of values in the vertex attribute
   * third argument: data type
   * forth argument: whether the values should be normalized
   * fifth argument: the space between the start of two consecutive vertex attributes in the array in bytes (also known as stride)
   * sixth argument: offset of where the position data begins (it has to be cast into a void pointer for some reason, idk why)
  */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Generate Texture
  unsigned int texture;
  glGenTextures(1, &texture);

  // Clamp the texture so that anything outside the texture will be a user defined color
  /**
   * first argument: texture target (21d, 2d, 3d, etc...)
   * second argument: which axis we are configuring (axes: s, t, r(if using 3d textures))
   * third argument: texture wrapping mode we would like
  */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // Specify user defined border color
  float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  // Specify the filtering mode
  /**
   * the filtering mode the color is at a point on the texture relative to the colors of the four texels that surround it
   * nearest neighbour or point filtering returns the color of the nearest texel
   * bilinear filtering linearly interpolates between the four colors to get a ratiod mix of the four
  */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Bind texture
  glBindTexture(GL_TEXTURE_2D, texture);

  // Read image file
  int width, height, numChannels;
  unsigned char* imageData = stbi_load("textures/crate-texture1024x1024.png", &width, &height, &numChannels, 0);

  // Error handling
  if (imageData)
  {
    // TODO: Explain this
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cerr << "Failed to load texture" << std::endl;
  }

  // Cleanup
  stbi_image_free(imageData);

  // Compile and link shaders
  unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, readFile("shaders/vertex-shader.glsl"));
  unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, readFile("shaders/fragment-shader.glsl"));
  unsigned int shaderProgram = createProgram(vertexShader, fragmentShader);

  // Use the program we just created
  glUseProgram(shaderProgram);

  while(!glfwWindowShouldClose(window))
  {
    update(window);

      // Clear screen using clear color
      glClear(GL_COLOR_BUFFER_BIT);

      // Initialize transform matrix
      glm::mat4 trans = glm::mat4(1);

      /**
       * A hard thing to get used to in OpenGL is that the last transformation you write in code is applied first
       * This is because matrices are multiplied from right to left
       * 
       * Say you applied a rotation, then a translation,
       * the program writes it down as:
       * --------------------------------------------------------------------------------------------------------
       * | transform_matrx = R * T
       * --------------------------------------------------------------------------------------------------------
       * and if you multiply that with a vector, it becomes:
       * --------------------------------------------------------------------------------------------------------
       * | vector = R * T * v
       * --------------------------------------------------------------------------------------------------------
       * ... which is equal to ...
       * --------------------------------------------------------------------------------------------------------
       * | R * (T * v)
       * --------------------------------------------------------------------------------------------------------
       * 
       * It's hard to remember, but that's just how it is `\_('_')_/`
       */

      // Translate transform matrix
      trans = glm::translate(trans, glm::vec3(std::sin(glfwGetTime() * 1.5) / 2, 0.0, 0.0));

      // Rotate transform matrix
      trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0, 0, 1));

      // Create uniform for that matrix
      /**
       * first argument: uniform location
       * second argument: how many matrices we would like to send
       * third argument: if we would like to transpose out matrix (swap columns and rows)
       * fourth argument: convert to OpenGL format
       */
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_transformMatrix"), 1, GL_FALSE, glm::value_ptr(trans));

      // Draw Vertex Arrays
      /**
       * frist argument: the type of primitive we would like to draw
       * second argument: the index of the vertex array
       * third argument: the number of vertices to draw
       */
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Swaps front and back buffers
    /**
     * An application takes time to draw all the pixels on the screen
     * When an application draws on a single buffer, it can have a flickering effect as not all pixels have been drawn when the screen renders
     * To solve this issue, applications implement double buffers
     * The front buffer contains the final output image shown on the screen
     * All the rendering commands draw to the back buffer
     * As soon as the back buffer is complete, they swap, instantaneously changing frames without flickering
     */
    glfwSwapBuffers(window);

    // Checks if any events are triggered (keyboard, mouse, etc)
    glfwPollEvents();
  }

  // Proper cleanup
  glfwTerminate();

  return 0;
}
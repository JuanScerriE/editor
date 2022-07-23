/*
 * - Open files.
 * - Cursor view for the file.
 * - A render cursor view.
 * - An event loop.
 * - A input handler.
 * - A buffer structure to add all the things and output all
 * at once.
 * - A way to translate between the actual view and the
 * render view. (Rendering is important to output
 * non-printable characters.)
 * - Input should be effecting the actual view then we
 * convert to the render view.
 * - Some backend to create the the term-ui easily.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>


struct Vec2 {
  float x;
  float y;
};

struct Vec3 {
  float x;
  float y;
  float z;
};

void framebuffer_size_callback(GLFWwindow *window,
                               int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

static int parseShader(const std::string &filePath) {
  std::ifstream stream(filePath);

  if (!stream.is_open()) {
    return -1;
  }

  enum ShaderType {
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1,
  };

  std::string line;
  std::stringstream ss[2];
  ShaderType type = ShaderType::NONE;

  while (std::getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        type = ShaderType::VERTEX;
      } else if (line.find("fragment") !=
                 std::string::npos) {
        type = ShaderType::FRAGMENT;
      }
    } else if (type != ShaderType::NONE) {
      ss[type] << line << "\n";
    }
  }

  return 0;
}

static unsigned int compileShader(
  unsigned int type, const std::string &source) {
  unsigned int id = glCreateShader(type);

  // String must still exits here.
  const char *src = source.c_str();
  glShaderSource(id, 1, &src, NULL);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    std::cerr << "Faile to compile "
              << (type == GL_VERTEX_SHADER ? "vertex"
                                           : "fragment")
              << "shader" << std::endl;
    std::cerr << message << std::endl;
    glDeleteShader(id);
    return 0;
  }

  return id;
}

static unsigned int createShader(
    const std::string &vertexShader,
    const std::string &fragmentShader) {
  unsigned int program = glCreateProgram();
  // TODO: handle error
  unsigned int vs =
      compileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs =
      compileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

int main() {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit()) {
    std::cerr << "Failed to initialise GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  /* Create a windowed mode window and its OpenGL context
   */
  window =
      glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create GLFW window"
              << std::endl;
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  // This needs to execute before the context
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialise GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window,
                                 framebuffer_size_callback);

  std::cout << glGetString(GL_VERSION) << std::endl;

  Vec3 positions[3] = {{-0.5f, -0.5f, 0.0f},
                       {0.5f, -0.5f, 0.0f},
                       {0.0f, 0.5f, 0.0f}};

  unsigned int buffer;
  unsigned int vao;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &buffer);

  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vec3), positions,
               GL_STATIC_DRAW);

  glBindVertexArray(vao);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vec3), 0);
  glEnableVertexAttribArray(0);

  std::string vertexShader =
      "#version 460 core\n"
      "layout(location = 0) in vec3 aPos;\n"
      "void main() {\n"
      "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, "
      "1.0);\n"
      "}\n";

  std::string fragmentShader =
      "#version 460 core\n"
      "layout(location = 0) out vec4 color;\n"
      "void main() {\n"
      "    color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
      "}\n";
  unsigned int shader =
      createShader(vertexShader, fragmentShader);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    processInput(window);

    // We do all the rendering here

    // We are going to draw the one which is being binded.
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // End of rendering

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}

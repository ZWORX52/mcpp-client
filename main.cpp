// NOLINT(legal/copyright)
#include "./main.hpp"

const char *vertex_shader_source =
    "#version 410 core\n"
    "layout (location = 0) in vec3 a_pos;\n"
    "void main() {\n"
    "    gl_Position = vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);\n"
    "}";

const char *fragment_shader_source =
    "#version 410 core\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "    frag_color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
    "}";

void error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error #%d: %s\n", error, description);
}

// I'll be handling inputs in the tick thread for the user depending on the
// current game state. (distant: ENGLISH, PLEASE!) Fine. Every game tick, the
// client will poll and process inputs in singleplayer. In multiplayer, the
// client will send inputs every client tick. Happy? Good. Note to self: use
// if (glfwGetKey(window, GLFW_KEY_<key name>)) { ... }
/*
static void key_callback(__attribute__((unused)) GLFWwindow *window, int key,
                         int scancode, int action, int modifiers) {
  std::cout << "Key: " << key << ", scancode: " << scancode
            << ", action: " << action << ", modifiers: " << modifiers
            << std::endl;
}
*/

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window,
                               int width, int height) {
  glViewport(0, 0, width, height);
}

int check_shader_for_errors(unsigned int shader) {
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[1024] = {0};
    glGetShaderInfoLog(shader, 1024, NULL, info_log);
    std::cout << "Error: Shader compilation failed" << std::endl
              << info_log << std::endl;
  }
  return success;
}

int check_program_for_errors(unsigned int shader) {
  int success;
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if (!success) {
    char info_log[1024] = {0};
    glGetProgramInfoLog(shader, 1024, NULL, info_log);
    std::cout << "Error: Program linking failed" << std::endl
              << info_log << std::endl;
  }
  return success;
}

int main() {
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    return 1;
  }
  std::cout << "GLFW initialized" << std::endl;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  GLFWwindow *window = glfwCreateWindow(640, 480, "MCPP Client", NULL, NULL);
  if (!window) {
    std::cout << "Failed to create window" << std::endl;
    int error = glfwGetError(NULL);
    if (error == GLFW_VERSION_UNAVAILABLE) {
      std::cout
          << "Hint: your computer doesn't appear to support OpenGL version 4.1!"
          << std::endl;
    }
    return 1;
  }
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return 1;
  }
  glfwSwapInterval(1);
  glViewport(0, 0, 640, 480);

  // build and compile shaders
  // -------------------------
  // 1. vertex shader
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  if (!check_shader_for_errors(vertex_shader))
    return 1;

  // 2. fragment shader
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  if (!check_shader_for_errors(fragment_shader))
    return 1;

  // 3. link em, baby
  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  if (!check_program_for_errors(shader_program))
    return 1;
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // vertex data
  float vertices[] = {-0.75f, -0.5f, 0.0f, -0.25f, -0.5f,
                      0.0f,   -0.5f, 0.5f, 0.0f};

  unsigned int vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);

  // above call to VertexAttribPointer registered "vbo" as "vao"'s bound vbo
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // however this isn't really necessary, nor is the above call, but it's only
  // useful for when we don't want to accidentally modify the vao. generally
  // unbinding vaos or vbos isn't necessary, because you will bind a new one
  // before making any modifing calls (which overwrites anyway).
  // glBindVertexArray(0);

  // rectangle vertices
  float rectangle_vertices[] = {0.75f, 0.5f,  0.0f, 0.75f, -0.5f, 0.0,
                                0.25f, -0.5f, 0.0f, 0.25f, 0.5f,  0.0f};
  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  unsigned int rectangle_vao, rectangle_vbo, rectangle_ebo;
  glGenVertexArrays(1, &rectangle_vao);
  glGenBuffers(1, &rectangle_vbo);
  glGenBuffers(1, &rectangle_ebo);
  // case in point
  glBindVertexArray(rectangle_vao);

  glBindBuffer(GL_ARRAY_BUFFER, rectangle_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // uncomment for wireframes
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    // Finally, a window we can use! Yay!
    // *reads "Hello Triangle" on learnopengl.com* Oh no...

    // rendering
    // ---------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    // i don't need to do this every frame, however it's more concise so hah
    glBindVertexArray(vao);
    // FINALLY A TRIANGLE
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(rectangle_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // do the fancy that I don't have to worry about thank goodness ;)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  std::cout << "Stopping!" << std::endl;
  glfwDestroyWindow(window);
  glfwTerminate();
}

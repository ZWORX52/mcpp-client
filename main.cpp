// NOLINT(legal/copyright)
#include "./main.hpp"

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

unsigned int load_shader(const std::string &path_vertex,
                         const std::string &path_fragment) {
  std::string vertex_code, fragment_code;
  std::ifstream vertex_file, fragment_file;

  // throw exceptions please
  vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    vertex_file.open(path_vertex);
    fragment_file.open(path_fragment);
    std::stringstream vertex_stream, fragment_stream;
    // read files
    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();
    vertex_file.close();
    fragment_file.close();
    vertex_code = vertex_stream.str();
    fragment_code = fragment_stream.str();
  } catch (const std::ifstream::failure &e) {
    std::cout << "Failed reading shader files " << path_vertex << " and "
              << path_fragment << " (error code #" << e.code()
              << "):" << std::endl
              << e.what() << std::endl;
    return 0;
  }

  int success;
  char log[1024];

  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  // Because you can't take the address of an rvalue :P
  const char *vertex_code_c_str = vertex_code.c_str();
  glShaderSource(vertex_shader, 1, &vertex_code_c_str, NULL);
  glCompileShader(vertex_shader);
  // any compile errors?
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 1024, NULL, log);
    std::cout << "Vertex shader compilation failed: " << log << std::endl;
    return 0;
  }

  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  const char *fragment_code_c_str = fragment_code.c_str();
  glShaderSource(fragment_shader, 1, &fragment_code_c_str, NULL);
  glCompileShader(fragment_shader);
  // any compile errors here?
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 1024, NULL, log);
    std::cout << "Fragment shader compilation failed: " << log << std::endl;
    return 0;
  }

  unsigned int program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader);
  glAttachShader(program_id, fragment_shader);
  glLinkProgram(program_id);
  // any linking errors?
  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_id, 1024, NULL, log);
    std::cout << "Program linking failed: " << log << std::endl;
    return 0;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return program_id;
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
  glEnable(GL_DEBUG_OUTPUT);

  unsigned int program =
      load_shader("../shaders/main.vert", "../shaders/main.frag");
  // perhaps i'm being run in the build dir, try up a dir?
  if (!program) {
    // load_shader returns 0 on failure. weird, i know, but other returns are
    // valid and we need a uint sooo
    program = load_shader("./shaders/main.vert", "./shaders/main.frag");
    std::cout << "Note: shaders were found in non-default location ./shaders/ "
                 "(instead of ../shaders/)"
              << std::endl;
  }

  // vertex data
  // format: x, y, z, r, g, b, s, t, etc, etc
  float attributes[] = {0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,
                        0.5f,  -0.5f, 0.0f, 0.25f, 1.0f, 1.0f, 1.0f, 0.0f,
                        -0.5f, -0.5f, 0.0f, 0.5f,  1.0f, 1.0f, 0.0f, 0.0f,
                        -0.5f, 0.5f,  0.0f, 0.75f, 1.0f, 1.0f, 0.0f, 1.0f};

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  unsigned int vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(attributes), attributes, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // textures
  unsigned int tex1;
  glGenTextures(1, &tex1);
  glBindTexture(GL_TEXTURE_2D, tex1);
  // set to default wrapping method
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // load image into texture and gen mipmaps
  // ha ha qoi go BRRRRRRRRRRRRRRRRR
  qoi_desc desc;
  void *pixels = qoi_read("../assets/container.qoi", &desc, 0);
  if (pixels) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, desc.width, desc.height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  free(pixels);

  unsigned int tex2;
  glGenTextures(1, &tex2);
  glBindTexture(GL_TEXTURE_2D, tex2);
  // set to default wrapping method
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // load image into texture and gen mipmaps
  // ha ha qoi go BRRRRRRRRRRRRRRRRR
  pixels = qoi_read("../assets/awesomeface.qoi", &desc, 0);
  if (pixels) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, desc.width, desc.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  free(pixels);

  // tell opengl what texture unit each sampler2D (in frag shader) belongs to
  glUseProgram(program);
  glUniform1i(glGetUniformLocation(program, "texture1"), 0);
  glUniform1i(glGetUniformLocation(program, "texture2"), 1);

  // uncomment for wireframes
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  std::vector<float> frame_times;
  while (!glfwWindowShouldClose(window)) {
    // Finally, a window we can use! Yay!
    // *reads "Hello Triangle" on learnopengl.com* Oh no...o
    float start = glfwGetTime();

    // rendering
    // ---------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glUseProgram(program);

    // update the color uniform
    float time = glfwGetTime();
    float hue = fmod(time * 0.25f, 360.0f);
    glUniform3f(glGetUniformLocation(program, "hue"), hue, 1.0f, 1.0f);

    // i don't need to do this every frame, however it's more concise so hah
    glBindVertexArray(vao);
    // FINALLY A TRIANGLE
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    float end = glfwGetTime();
    frame_times.push_back(end - start);
    // do the fancy that I don't have to worry about thank goodness ;)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  double sum = std::accumulate(frame_times.begin(), frame_times.end(), 0.0);
  std::cout << "sum: " << sum << std::endl;
  std::cout << "Average frame time: " << sum / frame_times.size() << std::endl;

  // optional: these resources are done, we can dealloc them if we want
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);

  std::cout << "Stopping!" << std::endl;
  glfwDestroyWindow(window);
  glfwTerminate();
}

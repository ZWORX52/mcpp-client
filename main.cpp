// NOLINT(legal/copyright)
#include "./main.hpp"

void error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error #%d: %s\n", error, description);
}

// I'll be handling inputs in the tick thread for the user depending on the
// current game state. (distant: ENGLISH, PLEASE!) Fine. Every tick, the client
// will poll and process inputs in singleplayer. In multiplayer, the client will
// send inputs every client tick. Happy? Good.
// Note to self: use if (glfwGetKey(window, GLFW_KEY_<key name>)) { ... }
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

  uint64_t frames = 0;
  while (!glfwWindowShouldClose(window)) {
    // Finally, a window we can use! Yay! *reads "Hello Triangle" on
    // learnopengl.com* Oh no...
    frames++;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  std::cout << "Stopping!" << std::endl;
  std::cout << "Frame count: " << frames << std::endl;
  glfwDestroyWindow(window);
  glfwTerminate();
}

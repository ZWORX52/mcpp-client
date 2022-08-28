// NOLINT(legal/copyright)
#include "./main.hpp"

void error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error #%d: %s\n", error, description);
}

static void key_callback(__attribute__((unused)) GLFWwindow *window, int key,
                         int scancode, int action, int modifiers) {
  std::cout << "Key: " << key << ", scancode: " << scancode
            << ", action: " << action << ", modifiers: " << modifiers
            << std::endl;
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
  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return 1;
  }

  uint64_t frames = 0;
  while (!glfwWindowShouldClose(window)) {
    // Finally, a window we can use! Yay!
    frames++;
  }

  std::cout << "Stopping!" << std::endl;
  std::cout << "Frame count: " << frames << std::endl;
  glfwDestroyWindow(window);
  glfwTerminate();
}

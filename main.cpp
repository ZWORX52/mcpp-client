// NOLINT(legal/copyright)
#include "./main.hpp"

int main() {
  std::cout << "MCPP client version: " << mcpp_client_VERSION_MAJOR << "."
            << mcpp_client_VERSION_MINOR << std::endl;
  std::cout << "Initializing GLFW..." << std::endl;
  if (!glfwInit()) {
    std::cout << "Initialization failed :(" << std::endl;
    return 1;
  }
  std::cout << "Initialization succeeded!" << std::endl;

  std::cout << "Terminating GLFW..." << std::endl;
  glfwTerminate();
  std::cout << "All shut down!" << std::endl;
}

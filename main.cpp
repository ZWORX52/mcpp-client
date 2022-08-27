// NOLINT(legal/copyright)
#include <iostream>

#include "version.hpp"

int main() {
  std::cout << "Using version " << mcpp_client_VERSION_MAJOR << "."
            << mcpp_client_VERSION_MINOR << std::endl;
}

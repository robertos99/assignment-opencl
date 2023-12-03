#include "lib.hpp"

int main() {
  clsetup();
  processImage("resources/cattest.jpg", "resources/cattest2.jpg");
  processImage2("resources/cattest.jpg", "resources/cattest3.jpg");
  return 0;
}

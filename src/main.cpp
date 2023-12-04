#include "lib.hpp"

int main() {
  process_image_imgToYCbCr_opencv("resources/cattest.jpg",
                                  "resources/cattest_ycbcr_opencv.jpg");
  process_image_imgToYCbCr_cpp("resources/cattest.jpg",
                               "resources/cattest_ycbcr_cpp.jpg");
  process_image_imgToYCbCr_opencl("resources/cattest.jpg",
                                  "resources/cattest_ycbcr_opencl.jpg");

  // process_image_imgToDilation_cpp("resources/cattest.jpg",
  //                                 "resources/cattest_dilated_cpp.jpg");
  return 0;
}

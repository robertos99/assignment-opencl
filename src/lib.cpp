#include <fstream>
#include <iostream>
#include <string>

#include "stb_image.h"
#include "stb_image_write.h"

// OpenCL includes
#include <CL/opencl.hpp>

#include "lib.hpp"

std::string read_kernel(const std::string& filename);

int load_image() {
  int width = 0;
  int height = 0;
  int channels = 0;
  unsigned char* img =
      stbi_load("resources/test.png", &width, &height, &channels, 0);
  if (img == NULL) {
    std::cerr << "Error in loading the image\n" << std::endl;
    return EXIT_FAILURE;
  }

  // Write the image back to disk
  if (!stbi_write_png("resources/output_image.png", width, height, channels,
                      img, width * channels)) {
    std::cerr << "Error in saving the image\n" << std::endl;
    stbi_image_free(img);
    return EXIT_FAILURE;
  }

  // Free the image memory
  stbi_image_free(img);

  std::cout << "Image processing complete. Output saved as output_image.png.\n"
            << std::endl;
  return EXIT_SUCCESS;
}

void clsetup() {
  std::vector<cl::Platform> all_platforms;
  cl::Platform::get(&all_platforms);

  std::vector<cl::Device> all_devices;
  all_platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &all_devices);
  std::cout << "These are the devices available: " << std::endl;
  for (int i = 0; i < all_devices.size(); i++) {
    std::cout << "Device #" << i
              << ", name: " << all_devices[i].getInfo<CL_DEVICE_NAME>()
              << std::endl;
  }

  cl::Context context({all_devices[0]});
  cl::CommandQueue queue(context, all_devices[0]);

  std::string kernel_source_code = read_kernel("./src/kernel.cl");
  cl::Program::Sources sources;
  sources.push_back({kernel_source_code.c_str(), kernel_source_code.length()});

  cl::Program program(context, sources);
  if (program.build({all_devices[0]}) != CL_SUCCESS) {
    std::cerr << "Cannot build the program!" << std::endl;
    std::cerr << "Build log: \n"
              << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(all_devices[0])
              << std::endl;
    exit(1);
  }
  cl::Kernel vectorAddKernel(program, "vectorAdd");

  int size = 1024;
  int* a = new int[size];
  int* b = new int[size];
  int* c = new int[size];

  for (int i = 0; i < size; i++) {
    a[i] = 1;
    b[i] = 2;
  }

  cl::Buffer dev_a(context, CL_MEM_READ_ONLY, sizeof(int) * size);
  cl::Buffer dev_b(context, CL_MEM_READ_ONLY, sizeof(int) * size);
  cl::Buffer dev_c(context, CL_MEM_READ_WRITE, sizeof(int) * size);

  queue.enqueueWriteBuffer(dev_a, CL_TRUE, 0, sizeof(int) * size, a);
  queue.enqueueWriteBuffer(dev_b, CL_TRUE, 0, sizeof(int) * size, b);

  vectorAddKernel.setArg(0, dev_a);
  vectorAddKernel.setArg(1, dev_b);
  vectorAddKernel.setArg(2, dev_c);
  vectorAddKernel.setArg(3, size);

  queue.enqueueNDRangeKernel(vectorAddKernel, cl::NullRange, cl::NDRange(1024),
                             cl::NDRange(4, 4, 1));

  queue.enqueueReadBuffer(dev_c, CL_TRUE, 0, sizeof(int) * size, c);

  queue.finish();

  float avg = 0;

  for (int i = 0; i < size; i++) {
    int ci = c[i];
    avg += ci;
  }

  avg /= size;
  std::cout << "Average is: " << avg << ", should be: 3.0" << std::endl;
}

std::string read_kernel(const std::string& filename) {
  std::string kernel_text;
  std::ifstream kernel_reader;
  kernel_reader.open(filename, std::ios::in);
  std::string line;
  while (std::getline(kernel_reader, line)) {
    kernel_text.append(line);
    kernel_text.append("\n");
  }
  kernel_reader.close();
  return kernel_text;
}

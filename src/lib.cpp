#include <fstream>
#include <iostream>
#include <string>

#include "opencv2/opencv.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
// OpenCL includes
#include <CL/opencl.hpp>

#include "lib.hpp"

int load_image(char* imagepath) {
  int width = 0;
  int height = 0;
  int channels = 0;
  unsigned char* img = stbi_load(imagepath, &width, &height, &channels, 0);
  if (img == nullptr) {
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
  std::unique_ptr<int[]> a(new int[size]);
  std::unique_ptr<int[]> b(new int[size]);
  std::unique_ptr<int[]> c(new int[size]);

  for (int i = 0; i < size; i++) {
    a[i] = 1;
    b[i] = 2;
  }

  cl::Buffer dev_a(context, CL_MEM_READ_ONLY, sizeof(int) * size);
  cl::Buffer dev_b(context, CL_MEM_READ_ONLY, sizeof(int) * size);
  cl::Buffer dev_c(context, CL_MEM_READ_WRITE, sizeof(int) * size);

  cl::CommandQueue queue(context, all_devices[0]);
  queue.enqueueWriteBuffer(dev_a, CL_TRUE, 0, sizeof(int) * size, a.get());
  queue.enqueueWriteBuffer(dev_b, CL_TRUE, 0, sizeof(int) * size, b.get());

  vectorAddKernel.setArg(0, dev_a);
  vectorAddKernel.setArg(1, dev_b);
  vectorAddKernel.setArg(2, dev_c);
  vectorAddKernel.setArg(3, size);

  queue.enqueueNDRangeKernel(vectorAddKernel, cl::NullRange, cl::NDRange(1024),
                             cl::NDRange(4, 4, 1));

  queue.enqueueReadBuffer(dev_c, CL_TRUE, 0, sizeof(int) * size, c.get());

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

char rgbToYCbCr(char R, char G, char B) {
  char Y = 0.299 * R + 0.587 * G + 0.114 * B;
  char Cb = -0.168736 * R - 0.331264 * G + 0.5 * B + 128;
  char Cr = 0.5 * R - 0.418688 * G - 0.081312 * B + 128;
  return Y;
}

void processImage(char* imgpath, char* writeToPath) {
  // read input image
  cv::Mat inputImage = cv::imread(imgpath, cv::IMREAD_UNCHANGED);
  if (inputImage.empty()) {
    std::cerr << "Error: Unable to open image file." << std::endl;
    return;
  }
  if (inputImage.empty()) {
    std::cerr << "Error: Unable to open image file." << std::endl;
    return;
  }
  cv::Mat yCbCrImage;
  cv::cvtColor(inputImage, yCbCrImage, cv::COLOR_BGR2YCrCb);
  cv::imwrite(writeToPath, yCbCrImage);
}

void convertBGRToYCbCr(ImageData& input) {
  for (int y = 0; y < input.height; ++y) {
    for (int x = 0; x < input.width; ++x) {
      int idx = y * input.width + x;

      uchar blue = input.blueChannel[idx];
      uchar green = input.greenChannel[idx];
      uchar red = input.redChannel[idx];

      uchar Y = static_cast<uchar>(0.114 * blue + 0.587 * green + 0.299 * red);
      uchar Cb =
          static_cast<uchar>(-0.081 * blue - 0.419 * green + 0.500 * red + 128);
      uchar Cr =
          static_cast<uchar>(0.500 * blue - 0.331 * green - 0.169 * red + 128);

      input.blueChannel[idx] = Y;    // Y in the blue channel
      input.greenChannel[idx] = Cb;  // Cb in the green channel
      input.redChannel[idx] = Cr;    // Cr in the red channel
    }
  }
}

void convertBGRToYCbCr2(uchar* blueChannel, uchar* greenChannel,
                        uchar* redChannel, int height, int width) {
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int idx = y * width + x;

      uchar blue = blueChannel[idx];
      uchar green = greenChannel[idx];
      uchar red = redChannel[idx];

      uchar Y = static_cast<uchar>(0.114 * blue + 0.587 * green + 0.299 * red);
      uchar Cb =
          static_cast<uchar>(-0.081 * blue - 0.419 * green + 0.500 * red + 128);
      uchar Cr =
          static_cast<uchar>(0.500 * blue - 0.331 * green - 0.169 * red + 128);

      // this isnt accurate. the names dont fit the RGB or BGR or whatver
      blueChannel[idx] = Y;
      greenChannel[idx] = Cb;
      redChannel[idx] = Cr;
    }
  }
}

ImageData parseMatToImageData(const cv::Mat& mat) {
  ImageData imgData;
  imgData.width = mat.cols;
  imgData.height = mat.rows;
  imgData.allocate(imgData.width, imgData.height, mat.channels() == 4);

  for (int y = 0; y < mat.rows; ++y) {
    for (int x = 0; x < mat.cols; ++x) {
      int idx = y * mat.cols + x;
      cv::Vec3b pixel = mat.at<cv::Vec3b>(y, x);
      imgData.blueChannel[idx] = pixel[0];
      imgData.greenChannel[idx] = pixel[1];
      imgData.redChannel[idx] = pixel[2];
      if (mat.channels() == 4) {
        imgData.alphaChannel[idx] = mat.at<cv::Vec4b>(y, x)[3];
      }
    }
  }
  return imgData;
}

cv::Mat imageDataToMat(const ImageData& imgData) {
  cv::Mat mat(imgData.height, imgData.width,
              imgData.alphaChannel != nullptr ? CV_8UC4 : CV_8UC3);
  for (int y = 0; y < mat.rows; ++y) {
    for (int x = 0; x < mat.cols; ++x) {
      int idx = y * mat.cols + x;
      cv::Vec3b& pixel = mat.at<cv::Vec3b>(y, x);
      pixel[0] = imgData.blueChannel[idx];
      pixel[1] = imgData.greenChannel[idx];
      pixel[2] = imgData.redChannel[idx];
      if (imgData.alphaChannel != nullptr) {
        mat.at<cv::Vec4b>(y, x)[3] = imgData.alphaChannel[idx];
      }
    }
  }
  return mat;
}

void processImage2(const std::string& imgpath, const std::string& writeToPath) {

  // OPENCV reads in BGR and not RGB. so the names of the buffers (blueChannel, ...) are wrong.
  // but im too lazy to correct.
  cv::Mat inputMat = cv::imread(imgpath, cv::IMREAD_UNCHANGED);
  if (inputMat.empty()) {
    std::cerr << "Error: Unable to open image file." << std::endl;
    return;
  }

  ImageData inputData = parseMatToImageData(inputMat);

  //convertBGRToYCbCr(inputData);
  convertBGRToYCbCr2(inputData.blueChannel, inputData.greenChannel,
                     inputData.redChannel, inputData.height, inputData.width);

  cv::Mat outputMat = imageDataToMat(inputData);

  cv::imwrite(writeToPath, outputMat);
}

void cl_convertBGRToYCbCr2(char* blueChannel, char* greenChannel,
                           char* redChannel, int height, int width) {

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

  std::string kernel_source_code = read_kernel("./src/bgrToYCbCr.cl");
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

  int size = height * width;

  cl::Buffer dev_a(context, CL_MEM_READ_ONLY, sizeof(int) * size);
  cl::Buffer dev_b(context, CL_MEM_READ_ONLY, sizeof(int) * size);
  cl::Buffer dev_c(context, CL_MEM_READ_WRITE, sizeof(int) * size);

  cl::CommandQueue queue(context, all_devices[0]);
  queue.enqueueWriteBuffer(dev_a, CL_TRUE, 0, sizeof(int) * size, blueChannel);
  queue.enqueueWriteBuffer(dev_b, CL_TRUE, 0, sizeof(int) * size, greenChannel);
  queue.enqueueWriteBuffer(dev_c, CL_TRUE, 0, sizeof(int) * size, greenChannel);

  vectorAddKernel.setArg(0, dev_a);
  vectorAddKernel.setArg(1, dev_b);
  vectorAddKernel.setArg(2, dev_c);
  vectorAddKernel.setArg(3, height);
  vectorAddKernel.setArg(3, width);

  queue.enqueueNDRangeKernel(vectorAddKernel, cl::NullRange, cl::NDRange(size),
                             cl::NDRange(4, 4, 1));

  queue.enqueueReadBuffer(dev_a, CL_TRUE, 0, sizeof(int) * size, blueChannel);
  queue.enqueueReadBuffer(dev_b, CL_TRUE, 0, sizeof(int) * size, greenChannel);
  queue.enqueueReadBuffer(dev_c, CL_TRUE, 0, sizeof(int) * size, greenChannel);

  queue.finish();
}
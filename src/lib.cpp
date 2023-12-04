#include <fstream>
#include <iostream>
#include <string>

#include <CL/opencl.hpp>
#include "opencv2/opencv.hpp"

#include "lib.hpp"

void ycbcr_cpp(uchar* blueChannel, uchar* greenChannel, uchar* redChannel,
               int height, int width);

void ycbcr_opencl(uchar* blueChannel, uchar* greenChannel, uchar* redChannel,
                  int height, int width);

void dilation_cpp(const uchar* inputBlue, const uchar* inputGreen,
                  const uchar* inputRed, uchar* outputBlue, uchar* outputGreen,
                  uchar* outputRed, int width, int height, int structElemSize);

void dilation_opencl(const uchar* inputBlue, const uchar* inputGreen,
                     const uchar* inputRed, uchar* outputBlue,
                     uchar* outputGreen, uchar* outputRed, int width,
                     int height, int structElemSize);

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

ImageData parseMatToImageData(const cv::Mat& mat) {
  ImageData imgData;
  imgData.width = mat.cols;
  imgData.height = mat.rows;
  imgData.allocate(imgData.width, imgData.height);

  for (int y = 0; y < mat.rows; ++y) {
    for (int x = 0; x < mat.cols; ++x) {
      int idx = y * mat.cols + x;
      cv::Vec3b pixel = mat.at<cv::Vec3b>(y, x);
      imgData.blueChannel[idx] = pixel[0];
      imgData.greenChannel[idx] = pixel[1];
      imgData.redChannel[idx] = pixel[2];
    }
  }
  return imgData;
}

cv::Mat imageDataToMat(const ImageData& imgData) {
  cv::Mat mat(imgData.height, imgData.width, CV_8UC3);
  for (int y = 0; y < mat.rows; ++y) {
    for (int x = 0; x < mat.cols; ++x) {
      int idx = y * mat.cols + x;
      cv::Vec3b& pixel = mat.at<cv::Vec3b>(y, x);
      pixel[0] = imgData.blueChannel[idx];
      pixel[1] = imgData.greenChannel[idx];
      pixel[2] = imgData.redChannel[idx];
    }
  }
  return mat;
}

void process_image_imgToYCbCr_opencv(char* imgpath, char* writeToPath) {
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

void process_image_imgToYCbCr_cpp(const std::string& imgpath,
                                  const std::string& writeToPath) {

  // OPENCV reads in BGR and not RGB. so the names of the buffers (blueChannel, ...) are wrong.
  // but im too lazy to correct.
  cv::Mat inputMat = cv::imread(imgpath, cv::IMREAD_UNCHANGED);
  if (inputMat.empty()) {
    std::cerr << "Error: Unable to open image file." << std::endl;
    return;
  }

  ImageData inputData = parseMatToImageData(inputMat);

  ycbcr_cpp(inputData.blueChannel, inputData.greenChannel, inputData.redChannel,
            inputData.height, inputData.width);

  cv::Mat outputMat = imageDataToMat(inputData);

  cv::imwrite(writeToPath, outputMat);
}

void process_image_imgToYCbCr_opencl(const std::string& imgpath,
                                     const std::string& writeToPath) {

  // OPENCV reads in BGR and not RGB. so the names of the buffers (blueChannel, ...) are wrong.
  // but im too lazy to correct.
  cv::Mat inputMat = cv::imread(imgpath, cv::IMREAD_UNCHANGED);
  if (inputMat.empty()) {
    std::cerr << "Error: Unable to open image file." << std::endl;
    return;
  }

  ImageData inputData = parseMatToImageData(inputMat);

  ycbcr_opencl(inputData.blueChannel, inputData.greenChannel,
               inputData.redChannel, inputData.height, inputData.width);

  cv::Mat outputMat = imageDataToMat(inputData);

  cv::imwrite(writeToPath, outputMat);
}

void ycbcr_cpp(uchar* blueChannel, uchar* greenChannel, uchar* redChannel,
               int height, int width) {
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int idx = y * width + x;

      uchar blue = blueChannel[idx];
      uchar green = greenChannel[idx];
      uchar red = redChannel[idx];

      auto Y = static_cast<uchar>(0.114 * blue + 0.587 * green + 0.299 * red);
      auto Cb =
          static_cast<uchar>(-0.081 * blue - 0.419 * green + 0.500 * red + 128);
      auto Cr =
          static_cast<uchar>(0.500 * blue - 0.331 * green - 0.169 * red + 128);

      // this isnt accurate. the names dont fit the RGB or BGR or whatver
      blueChannel[idx] = Y;
      greenChannel[idx] = Cb;
      redChannel[idx] = Cr;
    }
  }
}

void ycbcr_opencl(uchar* blueChannel, uchar* greenChannel, uchar* redChannel,
                  int height, int width) {

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

  int size = height * width;
  cl::Buffer dev_a(context, CL_MEM_READ_ONLY, sizeof(uchar) * size);
  cl::Buffer dev_b(context, CL_MEM_READ_ONLY, sizeof(uchar) * size);
  cl::Buffer dev_c(context, CL_MEM_READ_WRITE, sizeof(uchar) * size);
  cl::Buffer dev_h(context, CL_MEM_READ_WRITE, sizeof(int));
  cl::Buffer dev_w(context, CL_MEM_READ_WRITE, sizeof(int));
  cl::CommandQueue queue(context, all_devices[0]);
  queue.enqueueWriteBuffer(dev_a, CL_TRUE, 0, sizeof(uchar) * size,
                           blueChannel);
  queue.enqueueWriteBuffer(dev_b, CL_TRUE, 0, sizeof(uchar) * size,
                           greenChannel);
  queue.enqueueWriteBuffer(dev_c, CL_TRUE, 0, sizeof(uchar) * size, redChannel);
  queue.enqueueWriteBuffer(dev_h, CL_TRUE, 0, sizeof(int), &height);
  queue.enqueueWriteBuffer(dev_w, CL_TRUE, 0, sizeof(int), &width);

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
  cl::Kernel convertToYCbCrKernel(program, "convertBGRToYCbCr");
  convertToYCbCrKernel.setArg(0, dev_a);
  convertToYCbCrKernel.setArg(1, dev_b);
  convertToYCbCrKernel.setArg(2, dev_c);
  convertToYCbCrKernel.setArg(3, dev_h);
  convertToYCbCrKernel.setArg(4, dev_w);

  queue.enqueueNDRangeKernel(convertToYCbCrKernel, cl::NullRange,
                             cl::NDRange(1), cl::NullRange);

  queue.enqueueReadBuffer(dev_a, CL_TRUE, 0, sizeof(uchar) * size, blueChannel);
  queue.enqueueReadBuffer(dev_b, CL_TRUE, 0, sizeof(uchar) * size,
                          greenChannel);
  queue.enqueueReadBuffer(dev_c, CL_TRUE, 0, sizeof(uchar) * size, redChannel);

  queue.finish();
}

void process_image_imgToDilation_cpp(const std::string& imgpath,
                                     const std::string& writeToPath) {

  // OPENCV reads in BGR and not RGB. so the names of the buffers (blueChannel, ...) are wrong.
  // but im too lazy to correct.
  cv::Mat inputMat = cv::imread(imgpath, cv::IMREAD_UNCHANGED);
  if (inputMat.empty()) {
    std::cerr << "Error: Unable to open image file." << std::endl;
    return;
  }

  ImageData inputData = parseMatToImageData(inputMat);
  ImageData outputData;
  outputData.allocate(inputData.width, inputData.height);

  dilation_cpp(inputData.blueChannel, inputData.greenChannel,
               inputData.redChannel, outputData.blueChannel,
               outputData.greenChannel, outputData.redChannel, inputData.width,
               inputData.height, 30);

  cv::Mat outputMat = imageDataToMat(outputData);

  cv::imwrite(writeToPath, outputMat);
}

void process_image_imgToDilation_opencl(const std::string& imgpath,
                                        const std::string& writeToPath) {

  // OPENCV reads in BGR and not RGB. so the names of the buffers (blueChannel, ...) are wrong.
  // but im too lazy to correct.
  cv::Mat inputMat = cv::imread(imgpath, cv::IMREAD_UNCHANGED);
  if (inputMat.empty()) {
    std::cerr << "Error: Unable to open image file." << std::endl;
    return;
  }

  ImageData inputData = parseMatToImageData(inputMat);
  ImageData outputData;
  outputData.allocate(inputData.width, inputData.height);

  dilation_opencl(inputData.blueChannel, inputData.greenChannel,
                  inputData.redChannel, outputData.blueChannel,
                  outputData.greenChannel, outputData.redChannel,
                  inputData.width, inputData.height, 30);

  cv::Mat outputMat = imageDataToMat(outputData);

  cv::imwrite(writeToPath, outputMat);
}

void dilation_cpp(const uchar* inputBlue, const uchar* inputGreen,
                  const uchar* inputRed, uchar* outputBlue, uchar* outputGreen,
                  uchar* outputRed, int width, int height, int structElemSize) {

  // Perform dilation for each pixel
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      uchar maxValBlue = 0, maxValGreen = 0, maxValRed = 0;
      for (int ky = -structElemSize / 2; ky <= structElemSize / 2; ++ky) {
        for (int kx = -structElemSize / 2; kx <= structElemSize / 2; ++kx) {
          int ny = y + ky;
          int nx = x + kx;
          if (ny >= 0 && ny < height && nx >= 0 && nx < width) {
            int idx = ny * width + nx;
            maxValBlue = std::max(maxValBlue, inputBlue[idx]);
            maxValGreen = std::max(maxValGreen, inputGreen[idx]);
            maxValRed = std::max(maxValRed, inputRed[idx]);
          }
        }
      }
      int idx = y * width + x;
      outputBlue[idx] = maxValBlue;
      outputGreen[idx] = maxValGreen;
      outputRed[idx] = maxValRed;
    }
  }
}

void dilation_opencl(const uchar* inputBlue, const uchar* inputGreen,
                     const uchar* inputRed, uchar* outputBlue,
                     uchar* outputGreen, uchar* outputRed, int width,
                     int height, int structElemSize) {

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

  int size = height * width;
  cl::Buffer dev_a(context, CL_MEM_READ_ONLY, sizeof(uchar) * size);
  cl::Buffer dev_b(context, CL_MEM_READ_ONLY, sizeof(uchar) * size);
  cl::Buffer dev_c(context, CL_MEM_READ_WRITE, sizeof(uchar) * size);
  cl::Buffer dev_oa(context, CL_MEM_READ_ONLY, sizeof(uchar) * size);
  cl::Buffer dev_ob(context, CL_MEM_READ_ONLY, sizeof(uchar) * size);
  cl::Buffer dev_oc(context, CL_MEM_READ_WRITE, sizeof(uchar) * size);
  cl::Buffer dev_h(context, CL_MEM_READ_WRITE, sizeof(int));
  cl::Buffer dev_w(context, CL_MEM_READ_WRITE, sizeof(int));
  cl::Buffer dev_s(context, CL_MEM_READ_WRITE, sizeof(int));
  cl::CommandQueue queue(context, all_devices[0]);
  queue.enqueueWriteBuffer(dev_a, CL_TRUE, 0, sizeof(uchar) * size, inputBlue);
  queue.enqueueWriteBuffer(dev_b, CL_TRUE, 0, sizeof(uchar) * size, inputGreen);
  queue.enqueueWriteBuffer(dev_c, CL_TRUE, 0, sizeof(uchar) * size, inputRed);
  queue.enqueueWriteBuffer(dev_oa, CL_TRUE, 0, sizeof(uchar) * size,
                           outputBlue);
  queue.enqueueWriteBuffer(dev_ob, CL_TRUE, 0, sizeof(uchar) * size,
                           outputGreen);
  queue.enqueueWriteBuffer(dev_oc, CL_TRUE, 0, sizeof(uchar) * size, outputRed);
  queue.enqueueWriteBuffer(dev_h, CL_TRUE, 0, sizeof(int), &height);
  queue.enqueueWriteBuffer(dev_w, CL_TRUE, 0, sizeof(int), &width);
  queue.enqueueWriteBuffer(dev_s, CL_TRUE, 0, sizeof(int), &structElemSize);

  std::string kernel_source_code = read_kernel("./src/bgrDilation.cl");
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
  cl::Kernel dilate(program, "dilate");
  dilate.setArg(0, dev_a);
  dilate.setArg(1, dev_b);
  dilate.setArg(2, dev_c);
  dilate.setArg(3, dev_oa);
  dilate.setArg(4, dev_ob);
  dilate.setArg(5, dev_oc);
  dilate.setArg(6, dev_h);
  dilate.setArg(7, dev_w);
  dilate.setArg(8, dev_s);

  queue.enqueueNDRangeKernel(dilate, cl::NullRange, cl::NDRange(1),
                             cl::NullRange);

  queue.enqueueReadBuffer(dev_oa, CL_TRUE, 0, sizeof(uchar) * size, outputBlue);
  queue.enqueueReadBuffer(dev_ob, CL_TRUE, 0, sizeof(uchar) * size,
                          outputGreen);
  queue.enqueueReadBuffer(dev_oc, CL_TRUE, 0, sizeof(uchar) * size, outputRed);

  queue.finish();
}
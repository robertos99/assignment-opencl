#ifndef LIB_HPP
#define LIB_HPP

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "opencv2/opencv.hpp"

struct ImageData {
  uchar* blueChannel;
  uchar* greenChannel;
  uchar* redChannel;
  int width;
  int height;

  ImageData()
      : blueChannel(nullptr),
        greenChannel(nullptr),
        redChannel(nullptr),
        width(0),
        height(0) {}

  ~ImageData() {
    delete[] blueChannel;
    delete[] greenChannel;
    delete[] redChannel;
  }

  void allocate(int w, int h) {
    width = w;
    height = h;
    blueChannel = new uchar[width * height];
    greenChannel = new uchar[width * height];
    redChannel = new uchar[width * height];
  }
};

std::string read_kernel(const std::string& filename);

cv::Mat imageDataToMat(const ImageData& imgData);

ImageData parseMatToImageData(const cv::Mat& mat);

void process_image_imgToYCbCr_opencv(char* imgpath, char* writeToPath);

void process_image_imgToYCbCr_cpp(const std::string& imgpath,
                                  const std::string& writeToPath);

void process_image_imgToYCbCr_opencl(const std::string& imgpath,
                                     const std::string& writeToPath);

void process_image_imgToDilation_cpp(const std::string& imgpath,
                                     const std::string& writeToPath);
#endif

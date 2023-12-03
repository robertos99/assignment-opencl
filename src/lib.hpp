#ifndef LIB_HPP
#define LIB_HPP

#include <fstream>
#include <iostream>
#include <string>

#include "opencv2/opencv.hpp"

struct ImageData {
  uchar* blueChannel;
  uchar* greenChannel;
  uchar* redChannel;
  uchar* alphaChannel;  // Only if the image has an alpha channel
  int width;
  int height;

  ImageData()
      : blueChannel(nullptr),
        greenChannel(nullptr),
        redChannel(nullptr),
        alphaChannel(nullptr),
        width(0),
        height(0) {}

  ~ImageData() {
    delete[] blueChannel;
    delete[] greenChannel;
    delete[] redChannel;
    delete[] alphaChannel;
  }

  void allocate(int w, int h, bool hasAlpha = false) {
    width = w;
    height = h;
    blueChannel = new uchar[width * height];
    greenChannel = new uchar[width * height];
    redChannel = new uchar[width * height];
    if (hasAlpha) {
      alphaChannel = new uchar[width * height];
    }
  }
};

std::string read_kernel(const std::string& filename);

void processImage(char* imgpath, char* writeToPath);

int load_image(char* imagepath);

void clsetup();

void convertBGRToYCbCr(const ImageData& input);

cv::Mat imageDataToMat(const ImageData& imgData);

ImageData parseMatToImageData(const cv::Mat& mat);

void processImage2(const std::string& imgpath, const std::string& writeToPath);

void cl_convertBGRToYCbCr2(char* blueChannel, char* greenChannel,
                           char* redChannel, int height, int width);
#endif

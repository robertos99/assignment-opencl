// implement openmp functions here; with kernel and threadings etc ...
#include <iostream>

#include "openmp_lib.hpp"
#include "opencv2/opencv.hpp"

void yCbCr_opencv(std::string imgPath)
{
    cv::Mat srcImg = cv::imread(imgPath);
    cv::Mat YCbCrImg;

    if(srcImg.empty()){
        std::cout << "Could not read image path " << imgPath << std::endl;
        return;
    }

    cv::cvtColor(srcImg, YCbCrImg, cv::COLOR_YCrCb2RGB);
    cv::imwrite("out/yCbCr_out.jpg", YCbCrImg);

    return;
}

void dilatation_opencv(std::string imgPath)
{
    cv::Mat srcImg = cv::imread(imgPath);
    cv::Mat dilatationImg;

    if(srcImg.empty()){
        std::cout << "Could not read image path " << imgPath << std::endl;
        return;
    }

    cv::dilate(srcImg, dilatationImg, cv::Mat(), cv::Point(-1, -1), 5);
    cv::imwrite("out/dilatation_out.jpg", dilatationImg);

    return;
}

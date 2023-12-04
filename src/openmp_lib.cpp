// implement openmp functions here; with kernel and threadings etc ...
#include <iostream>

#include "openmp_lib.hpp"
#include "opencv2/opencv.hpp"

const char *yCbCr_opencv()
{
    std::string imgPath = "../images/animal/2.kitten_medium.jpg";
    cv::Mat img = cv::imread(imgPath, cv::COLOR_YCrCb2RGB);

    if(img.empty()){
        std::cout << "Could not read image path " << imgPath << std::endl;
        return nullptr;
    }

    cv::imwrite("../out/yCbCr_out.jpg", img);

    return nullptr;
}

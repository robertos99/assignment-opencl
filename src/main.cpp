// function calls from openmp_lib.cpp here
#include <iostream>

#include "openCL_lib.hpp"
int main()
{
    std::string imgPath = "images/animal/4.kitten_huge.jpg";
    // std::string dstPath;
    
    yCbCr_opencv(imgPath);
    dilatation_opencv(imgPath);
    yCbCr_opencl();
    return 0;
}

// function calls from openmp_lib.cpp here
#include <iostream>

#include "openCL_lib.hpp"

int main()
{
    std::string filepath = "images/animal/4.kitten_huge.jpg";
    // std::string dst_path;
    
    // yCbCrWithOpenCV(img_path);
    // dilatationWithOpenCV(img_path);
    convertToYCbCrInOpenCL(filepath);
    return 0;
}

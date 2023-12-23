// function calls from openmp_lib.cpp here
#include <iostream>

#include "openCL-lib.hpp"

int main()
{
    std::string filepath = "images/animal/1.kitten_small.jpg";
    // std::string dstPath = "out/";
    
    // BGRtoYCbCrWithOpenCV(filepath);
    // dilatationWithOpenCV(filepath);
    // convertToYCbCrWithOpenCL(filepath);
    // BGRtoYCbCrWithCPP(filepath);
    // func(filepath);
    foo(filepath);

    return 0;
}

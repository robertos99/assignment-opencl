#ifndef OPENCL_LIB_HPP
#define OPENCL_LIB_HPP

#include <string>

void BGRtoYCbCrWithOpenCV(std::string filepath);
void dilatationWithOpenCV(std::string filepath);
void yCbCr_self(std::string imgPath);
void convertToYCbCrWithOpenCL(std::string filepath);
void func(std::string filepath);
void BGRtoYCbCrWithCPP(std::string filepath);

void foo(std::string filepath);

#endif

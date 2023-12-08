// implement openmp functions here; with kernel and threadings etc ...
#include <iostream>
#include <fstream>

#include "openCL_lib.hpp"

#include "opencv2/opencv.hpp"
#include <CL/opencl.hpp>

void yCbCr_opencv(std::string img_path)
{
    cv::Mat src_img = cv::imread(img_path);
    cv::Mat YCbCrImg;

    if(src_img.empty()){
        std::cout << "Could not read image path " << img_path << std::endl;
        return;
    }

    cv::cvtColor(src_img, YCbCrImg, cv::COLOR_YCrCb2RGB);
    cv::imwrite("out/yCbCr_out.jpg", YCbCrImg);

    return;
}

void dilatation_opencv(std::string img_path)
{
    cv::Mat src_img = cv::imread(img_path);
    cv::Mat dilatation_img;

    if(src_img.empty()){
        std::cout << "Could not read image path " << img_path << std::endl;
        return;
    }

    cv::dilate(src_img, dilatation_img, cv::Mat(), cv::Point(-1, -1), 5);
    cv::imwrite("out/dilatation_out.jpg", dilatation_img); 

    return;
}

std::string read_kernel( const std::string& filename )
{
  std::string kernel_text;

  std::ifstream kernel_reader;
  kernel_reader.open(filename, std::ios::in);

  std::string line;
  while (std::getline( kernel_reader, line))
  {
    kernel_text.append( line );    
    kernel_text.append( "\n" );
  }
  kernel_reader.close();

  return kernel_text;
}


void yCbCr_opencl()
{
    std::vector<cl::Platform> all_platforms;                             // vector: "Vectors are sequence containers representing arrays that can change in size."
    cl::Platform::get(&all_platforms);                                   // get all platforms

    std::vector<cl::Device> all_devices;                                 // create device list
    cl::Platform selectedPlatform = all_platforms[0];                    // select platform
    selectedPlatform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);       // get all devices from first el in platforms list
    std::cout << "These are the devices available: " << std::endl;
    for (int i = 0; i < (int)all_devices.size(); i++) {
    std::cout << "Device #" << i
              << ", name: " << all_devices[i].getInfo<CL_DEVICE_NAME>()
              << std::endl;
    }

    cl::Device default_device = all_devices[0];                         // select gpu device

    // create command queue and context
    cl::Context context(default_device);                                // what does context do? 
    cl::CommandQueue queue(context, default_device);                    // queue is for ordering commands

    cl::Program::Sources sources;                                       // Sources is a list of pairs <kernel-code, string length>

    std::string kernel1 = read_kernel("kernel.cl");
    sources.push_back({kernel1.c_str(), kernel1.length()});

    cl::Program program(context, sources);
	if (program.build(default_device) != CL_SUCCESS)
    {
		std::cout << "Error building: " 
        << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		exit(1);
	}
}   
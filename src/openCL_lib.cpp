// implement openmp functions here; with kernel and threadings etc ...
#include <iostream>
#include <fstream>
#include <CL/opencl.hpp>

#include "opencv2/opencv.hpp"
#include "openCL_lib.hpp"

void yCbCrWithOpenCV(std::string filepath)
{
    cv::Mat img_data = cv::imread(filepath);
    cv::Mat YCbCr_img;

    if(img_data.empty()){
        std::cout << "Could not read image path " << filepath << std::endl;
        return;
    }

    cv::cvtColor(img_data, YCbCr_img, cv::COLOR_YCrCb2RGB);
    cv::imwrite("out/yCbCr_out.jpg", YCbCr_img);

    return;
}

void dilatationWithOpenCV(std::string filepath)
{
    cv::Mat img_data = cv::imread(filepath);
    cv::Mat dilatation_img;

    if(img_data.empty()){
        std::cout << "Could not read image path " << filepath << std::endl;
        return;
    }

    cv::dilate(img_data, dilatation_img, cv::Mat(), cv::Point(-1, -1), 5);
    cv::imwrite("out/dilatation_out.jpg", dilatation_img); 

    return;
}

std::string readKernel(const std::string& filename)
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

void convertToYCbCrInOpenCL(std::string filepath){
    cv::Mat img_data = cv::imread(filepath);

    // Loop over image data
    int height = img_data.rows;
    int width = img_data.cols;
    int depth = img_data.channels(); // bgr

    int size = height * width;

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            // get (h, w, b, g, r) for each pixel
            cv::Vec3b pixel = img_data.at<cv::Vec3b>(i, j);

            uint blue = pixel[0];
            uint green = pixel[1];
            uint red = pixel[2];
        }
    }
    
    // Query for available OpenCL platforms and devices
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);

    if(all_platforms.size() == 0){
        std::cout << " No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    cl::Platform selected_platform = all_platforms[0];
    std::vector<cl::Device> all_devices;
    selected_platform.getDevices(CL_DEVICE_TYPE_GPU, &all_devices);

    if(all_devices.size() == 0){
        std::cout << " No devices found. Check OpenCL installation!\n";
        exit(1);
    }

    std::cout << "Listing devices form " << selected_platform.getInfo<CL_PLATFORM_NAME>() << " (platform)" << std::endl;
    // std::cout << "Here is the list of available devices: " << std::endl;
    for(int i = 0; i < (int)all_devices.size(); i++){
        std::cout << "Device # " << i
        << ", name: " << all_devices[i].getInfo<CL_DEVICE_NAME>()
        << std::endl;
    }
    cl::Device selected_device = all_devices[0];
    std::cout << "Selecting this device: " << selected_device.getInfo<CL_DEVICE_NAME>() << "\n";

    // Create a context for one or more OpenCL devices in a platform
    cl::Context context(selected_device);
    cl::CommandQueue queue({context, selected_device});

    // Create and build programs for OpenCL devices in the context
    cl::Program::Sources sources;
    std::string kernel1 = readKernel("kernel.cl");

    // Select kernels to execute from the programs
    sources.push_back({kernel1.c_str(), kernel1.length()});
    cl::Program program(context, sources);
    if(program.build(selected_device) != CL_SUCCESS){
		std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(selected_device) << "\n";
		exit(1);
    }
    // Create memory objects for kernels to operate on
    cl::Buffer dev_blue(context, CL_MEM_READ_ONLY, sizeof(int) * size);
    cl::Buffer dev_green(context, CL_MEM_READ_ONLY, sizeof(int) * size);
    cl::Buffer dev_red(context, CL_MEM_READ_ONLY, sizeof(int) * size);
    cl::Buffer dev_height(context, CL_MEM_READ_ONLY, sizeof(int) * size);
    cl::Buffer dev_width(context, CL_MEM_READ_ONLY, sizeof(int) * size);
}

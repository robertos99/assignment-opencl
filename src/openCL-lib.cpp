// implement openmp functions here; with kernel and threadings etc ...
#include <iostream>
#include <fstream>
#include <CL/opencl.hpp>
#include <numeric>

#include "opencv2/opencv.hpp"
#include "openCL-lib.hpp"

// helper methods
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

void BGRtoYCbCrWithOpenCV(std::string filepath)
{
    cv::Mat img_data = cv::imread(filepath, cv::IMREAD_UNCHANGED);
    cv::Mat YCbCr_img;

    if(img_data.empty()){
        std::cout << "Could not read image path " << filepath << std::endl;
        exit(1);
    }

    cv::cvtColor(img_data, YCbCr_img, cv::COLOR_BGR2YCrCb);
    cv::imwrite("out/opencv-ycbcr-out.jpg", YCbCr_img);
}

void dilatationWithOpenCV(std::string filepath)
{
    cv::Mat img_data = cv::imread(filepath);
    cv::Mat dilatation_img;

    if(img_data.empty()){
        std::cout << "Could not read image path " << filepath << std::endl;
        exit(1);
    }

    cv::dilate(img_data, dilatation_img, cv::Mat(), cv::Point(-1, -1), 10);
    cv::imwrite("out/dilation-opencv-out.jpg", dilatation_img);
}

void BGRtoYCbCrWithCPP(std::string filepath){

    cv::Mat I = cv::imread(filepath, cv::IMREAD_UNCHANGED);
    cv::Mat O(I.size(), I.type());

    for(int y = 0; y < I.rows; y++){
        for(int x = 0; x < I.cols; x++){

            auto pixel= I.at<cv::Vec3b>(x, y);

            uint8_t blue_cn = pixel[0];
            uint8_t green_cn = pixel[1];
            uint8_t red_cn = pixel[2];

            uint8_t Y = (uint8_t)(0.1145 * (double)blue_cn + 0.5866 * (double)green_cn + 0.2989 * (double)red_cn);
            uint8_t Cb = (uint8_t)(-0.081 * (double)blue_cn - 0.419 * (double)green_cn + 0.500 * (double)red_cn + 128);
            uint8_t Cr = (uint8_t)(0.500 * (double)blue_cn - 0.331 * (double)green_cn - 0.169 * (double)red_cn + 128);

            O.at<cv::Vec3b>(x, y) = {Y, Cb, Cr};
        }
    }

    // cv::imwrite("out/opencv-ycbcr-out.jpg", I);
    cv::imwrite("out/cpp-ycbcr-out.jpg", O);
}

void func(std::string filepath){

    cv::Mat input_mat = cv::imread(filepath, cv::IMREAD_UNCHANGED);
    cv::Mat output_mat = cv::Mat(input_mat.size(), input_mat.type());

    int height = input_mat.rows;
    int width = input_mat.cols;

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if(platforms.empty()){
        std::cerr << " No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    cl::Platform default_platform = platforms.front();
    std::vector<cl::Device> devices;
    default_platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if(devices.empty()){
        std::cerr << " No devices found. Check OpenCL installation!\n";
        exit(1);
    }
    cl::Device default_device = devices.front();
    cl::Context context({default_device});
    cl::CommandQueue queue({context, default_device});

    cl::Program::Sources sources;
    std::string ycbcr_kernel_code = readKernel("ycbcr-kernel.cl");

    sources.push_back({ycbcr_kernel_code.c_str(), ycbcr_kernel_code.length()});
    cl::Program program(context, sources);
    if(program.build() != CL_SUCCESS){
        std::cerr << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device);
        exit(1);
    }
    cl::Kernel ycbcr_kernel(program, "BGRtoYCbCr");

    size_t size = width * height;
    cl::Buffer dev_input(context, CL_MEM_READ_ONLY, size * sizeof(uchar));
    cl::Buffer dev_output(context, CL_MEM_READ_WRITE, size * sizeof(uchar));

    ycbcr_kernel.setArg(0, dev_input);
    ycbcr_kernel.setArg(1, dev_output);
    ycbcr_kernel.setArg(2, height);
    ycbcr_kernel.setArg(3, width);

    int global_size = size;
    queue.enqueueWriteBuffer(dev_input, CL_TRUE, 0, size * sizeof(uchar), input_mat.data);
    queue.enqueueNDRangeKernel(ycbcr_kernel, cl::NullRange, cl::NDRange(global_size), cl::NullRange);
    queue.enqueueReadBuffer(dev_output, CL_TRUE, 0, size * sizeof(uchar), output_mat.data); 

    queue.finish();
    // speicher wieder freigeben

    // cv::imwrite("out/opencl-ycbcr-in.jpg", input_mat);
    // cv::imwrite("out/opencl-ycbcr-out.jpg", output_mat);

    std::cout << "input: (1x2) sample"<< std::endl << " "  << input_mat.rowRange(0, 1).colRange(0, 2) << std::endl << std::endl;
    std::cout << "output: (1x2) sample"<< std::endl << " "  << output_mat.rowRange(0, 1).colRange(0, 2) << std::endl << std::endl;
}

void foo(std::string filepath){

    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);

    std::vector<cl::Device> all_devices;
    all_platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &all_devices);

    std::cout << "These are the devices available: " << std::endl;
    for ( int i = 0; i < (int)all_devices.size(); i++ ){
        std::cout << "Device #" << i << ", name: "
                  << all_devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
    }

    cl::Device default_device = all_devices.front();
    cl::Context context({default_device});

    cl::CommandQueue queue(context, default_device);

    std::string kernel_source_code = readKernel("kernel.cl");

    cl::Program::Sources sources;
    sources.push_back({kernel_source_code.c_str(), kernel_source_code.length()});

    cl::Program program(context, sources);
    program.build({all_devices[0]});

    cl::Kernel ycbcr_kernel(program, "bgr_to_ycbcr");
    if ( program.build( { all_devices[0] } ) != CL_SUCCESS )
    {
        std::cerr << "Cannot build the program!" << std::endl;
        std::cerr << "Build log: \n"
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>( all_devices[0]) << std::endl;
        exit(1);
    }

    cv::Mat mat_bgr = cv::imread(filepath, cv::IMREAD_UNCHANGED);
    cv::Mat mat_ycbcr = cv::Mat(mat_bgr.size(), mat_bgr.type());

    size_t size = mat_bgr.total();

    std::vector<cv::Mat> bgr_channels;
    cv::split(mat_bgr, bgr_channels);

    // input arrays bgr 
    uchar* blue_cn = bgr_channels[0].data;
    uchar* green_cn = bgr_channels[1].data;
    uchar* red_cn = bgr_channels[2].data;

    // output arrays ycbcr
    std::vector<uchar> Y(size);
    std::vector<uchar> Cb(size);
    std::vector<uchar> Cr(size);

    cl::Buffer dev_blue(context, CL_MEM_READ_ONLY, size * sizeof(uchar), blue_cn);
    cl::Buffer dev_green(context, CL_MEM_READ_ONLY, size * sizeof(uchar), green_cn);
    cl::Buffer dev_red(context, CL_MEM_READ_ONLY, size * sizeof(uchar), red_cn);
    cl::Buffer dev_y(context, CL_MEM_WRITE_ONLY, sizeof(uchar) * Y.size());
    cl::Buffer dev_cb(context, CL_MEM_WRITE_ONLY, sizeof(uchar) * Cb.size());
    cl::Buffer dev_cr(context, CL_MEM_WRITE_ONLY, sizeof(uchar) * Cr.size());

    ycbcr_kernel.setArg(0, dev_blue);
    ycbcr_kernel.setArg(1, dev_green);
    ycbcr_kernel.setArg(2, dev_red);
    ycbcr_kernel.setArg(3, dev_y);
    ycbcr_kernel.setArg(4, dev_cb);
    ycbcr_kernel.setArg(5, dev_cr);

    queue.enqueueWriteBuffer(dev_blue, CL_TRUE, 0, size * sizeof(uchar), blue_cn);
    queue.enqueueWriteBuffer(dev_green, CL_TRUE, 0, size * sizeof(uchar), green_cn);
    queue.enqueueWriteBuffer(dev_green, CL_TRUE, 0, size * sizeof(uchar), red_cn);

    cl::size_type local_size[3] = {4, 4, 1};
    cl::size_type global_size[3] = {32, 32, 1};

    queue.enqueueNDRangeKernel(ycbcr_kernel, cl::NullRange, cl::NDRange(*global_size), cl::NDRange(*local_size));

    queue.enqueueReadBuffer(dev_y, CL_TRUE, 0, sizeof(uchar) * Y.size(), Y.data());
    queue.enqueueReadBuffer(dev_cb, CL_TRUE, 0, sizeof(uchar) * Cb.size(), Cb.data());
    queue.enqueueReadBuffer(dev_cr, CL_TRUE, 0, sizeof(uchar) * Cr.size(), Cr.data());

    queue.finish();

    cv::Mat ycbcr_channels[] = {
        cv::Mat(mat_bgr.size(), CV_8UC1, Y.data()),
        cv::Mat(mat_bgr.size(), CV_8UC1, Cb.data()),
        cv::Mat(mat_bgr.size(), CV_8UC1, Cr.data())
    };

    cv::merge(ycbcr_channels, 3, mat_ycbcr);

    cv::imwrite("out/opencl-ycbcr-in.jpg", mat_bgr);
    cv::imwrite("out/opencl-ycbcr-out.jpg", mat_ycbcr);

    std::cout << "input: (1x2) sample"<< std::endl << " "  << mat_bgr.rowRange(0, 1).colRange(0, 2) << std::endl << std::endl;
    std::cout << "output: (1x2) sample"<< std::endl << " "  << mat_ycbcr.rowRange(0, 1).colRange(0, 2) << std::endl << std::endl;
}

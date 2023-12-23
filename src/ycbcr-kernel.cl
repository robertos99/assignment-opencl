__kernel void BGRtoYCbCr(__global uchar* input, 
                         __global uchar* output, 
                         __global int* height; 
                         __global int* width){
    
    int x = get_global_id(0);
    int y = get_global_id(1);

    int loc = (y * width + x);

    if(x < width && y < height){
    
        uint8_t blue_cn = input[loc + 0];
        uint8_t green_cn = input[loc + 1];
        uint8_t red_cn = input[loc + 2];

        uint8_t Y = (uint8_t)(0.1145 * (double)blue_cn + 0.5866 * (double)green_cn + 0.2989 * (double)red_cn);
        uint8_t Cb = (uint8_t)(-0.081 * (double)blue_cn - 0.419 * (double)green_cn + 0.500 * (double)red_cn + 128);
        uint8_t Cr = (uint8_t)(0.500 * (double)blue_cn - 0.331 * (double)green_cn - 0.169 * (double)red_cn + 128);

        output[loc + 0] = Y;
        output[loc + 1] = Cb;
        output[loc + 2] = Cr;
    }
}

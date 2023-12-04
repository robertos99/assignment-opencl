__kernel void convertBGRToYCbCr(__global uchar* blueChannel,
                                __global uchar* greenChannel,
                                __global uchar* redChannel,
                                __global int* height, __global int* width) {
  for (int y = 0; y < *height; ++y) {
    for (int x = 0; x < *width; ++x) {
      int idx = y * (*width) + x;

      uchar blue = blueChannel[idx];
      uchar green = greenChannel[idx];
      uchar red = redChannel[idx];

      uchar Y = (uchar)(0.114 * blue + 0.587 * green + 0.299 * red);
      uchar Cb = (uchar)(-0.081 * blue - 0.419 * green + 0.500 * red + 128);
      uchar Cr = (uchar)(0.500 * blue - 0.331 * green - 0.169 * red + 128);

      // blueChannel[idx] = 1;   // Y in the blue channel
      // greenChannel[idx] = 1;  // Cb in the green channel
      // redChannel[idx] = 1;    // Cr in the red channel

      blueChannel[idx] = Y;    // Y in the blue channel
      greenChannel[idx] = Cb;  // Cb in the green channel
      redChannel[idx] = Cr;    // Cr in the red channel
    }
  }
}

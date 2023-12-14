__kernel void convertBGRToYCbCr(__global uchar* blueChannel,
                                __global uchar* greenChannel,
                                __global uchar* redChannel,
                                __global int* height, __global int* width,
                                __global int* maxPixelPerThreadX,
                                __global int* maxPixelPerThreadY) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  for (int pixelOffsetX = 0; pixelOffsetX < (*maxPixelPerThreadX);
       pixelOffsetX++) {
    for (int pixelOffsetY = 0; pixelOffsetY < (*maxPixelPerThreadY);
         pixelOffsetY++) {
      int pixelOfInterestX = x * (*maxPixelPerThreadX) + pixelOffsetX;
      int pixelOfInterestY = y * (*maxPixelPerThreadY) + pixelOffsetY;
      if (pixelOfInterestX < (*width) && pixelOfInterestY < (*height)) {
        int idx = pixelOfInterestY * (*width) + pixelOfInterestX;
        uchar blue = blueChannel[idx];
        uchar green = greenChannel[idx];
        uchar red = redChannel[idx];

        uchar Y = (uchar)(0.114 * blue + 0.587 * green + 0.299 * red);
        uchar Cb = (uchar)(-0.081 * blue - 0.419 * green + 0.500 * red + 128);
        uchar Cr = (uchar)(0.500 * blue - 0.331 * green - 0.169 * red + 128);

        blueChannel[idx] = Y;    // Y in the blue channel
        greenChannel[idx] = Cb;  // Cb in the green channel
        redChannel[idx] = Cr;    // Cr in the red channel
      }
    }
  }
}

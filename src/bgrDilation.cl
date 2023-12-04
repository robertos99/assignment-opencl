__kernel void dilate(__global const uchar* inputBlue,
                     __global uchar* inputGreen, __global uchar* inputRed,
                     __global uchar* outputBlue, __global uchar* outputGreen,
                     __global uchar* outputRed, __global int* width,
                     __global int* height, __global int* structElemSize) {
  for (int y = 0; y < (*height); ++y) {
    for (int x = 0; x < (*width); ++x) {
      uchar maxValBlue = 0, maxValGreen = 0, maxValRed = 0;
      for (int ky = -(*structElemSize) / 2; ky <= (*structElemSize) / 2; ++ky) {
        for (int kx = -(*structElemSize) / 2; kx <= (*structElemSize) / 2;
             ++kx) {
          int ny = y + ky;
          int nx = x + kx;
          if (ny >= 0 && ny < (*height) && nx >= 0 && nx < (*width)) {
            int idx = ny * (*width) + nx;
            maxValBlue = max(maxValBlue, inputBlue[idx]);
            maxValGreen = max(maxValGreen, inputGreen[idx]);
            maxValRed = max(maxValRed, inputRed[idx]);
          }
        }
      }
      int idx = y * (*width) + x;
      outputBlue[idx] = maxValBlue;
      outputGreen[idx] = maxValGreen;
      outputRed[idx] = maxValRed;
    }
  }
}

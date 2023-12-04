__kernel void dilate(__global const uchar* inputBlue,
                     __global uchar* inputGreen, __global uchar* inputRed,
                     __global uchar* outputBlue, __global uchar* outputGreen,
                     __global uchar* outputRed, __global int* width,
                     __global int* height, __global int* structElementSize) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  if (x < (*width) && y < (*height)) {
    uchar maxValBlue = 0, maxValGreen = 0, maxValRed = 0;
    for (int ky = -(*structElementSize) / 2; ky <= (*structElementSize) / 2;
         ++ky) {
      for (int kx = -(*structElementSize) / 2; kx <= (*structElementSize) / 2;
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
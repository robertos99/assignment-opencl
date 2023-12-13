__kernel void dilate(__global const uchar* inputBlue,
                     __global uchar* inputGreen, __global uchar* inputRed,
                     __global uchar* outputBlue, __global uchar* outputGreen,
                     __global uchar* outputRed, __global int* width,
                     __global int* height, __global int* structElementSize,
                     __global int* threadCount) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int pixelPerThread = ((*width) * (*height)) / (*threadCount);
  for (int pixelOffset = 0; pixelOffset <= pixelPerThread; pixelOffset++) {
    if (x + pixelOffset < (*width) && y < (*height)) {
      uchar maxValBlue = 0, maxValGreen = 0, maxValRed = 0;
      for (int pixelBoxY = -(*structElementSize) / 2;
           pixelBoxY <= (*structElementSize) / 2; ++pixelBoxY) {
        for (int pixelBoxX = -(*structElementSize) / 2;
             pixelBoxX <= (*structElementSize) / 2; ++pixelBoxX) {
          if (pixelBoxY + y >= 0 && pixelBoxY < (*height) &&
              pixelBoxX + x >= 0 && pixelBoxX + x + pixelOffset < (*width)) {
            int pixelIndexY = y + pixelBoxY;
            int pixelIndexX = x + pixelBoxX + pixelOffset;
            int idx = pixelIndexY * (*width) + pixelIndexX;
            maxValBlue = max(maxValBlue, inputBlue[idx]);
            maxValGreen = max(maxValGreen, inputGreen[idx]);
            maxValRed = max(maxValRed, inputRed[idx]);
          }
        }
      }
      int idx = y * (*width) + x + pixelOffset;
      outputBlue[idx] = maxValBlue;
      outputGreen[idx] = maxValGreen;
      outputRed[idx] = maxValRed;
    }
  }
}
__kernel void dilate(__global const uchar* inputBlue,
                     __global uchar* inputGreen, __global uchar* inputRed,
                     __global uchar* outputBlue, __global uchar* outputGreen,
                     __global uchar* outputRed, __global int* width,
                     __global int* height, __global int* structElementSize,
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
        uchar maxValBlue = 0, maxValGreen = 0, maxValRed = 0;
        for (int pixelBoxY = -(*structElementSize) / 2;
             pixelBoxY <= (*structElementSize) / 2; ++pixelBoxY) {
          for (int pixelBoxX = -(*structElementSize) / 2;
               pixelBoxX <= (*structElementSize) / 2; ++pixelBoxX) {
            if (pixelBoxY + pixelOfInterestY >= 0 &&
                pixelBoxY + pixelOfInterestY < (*height) &&
                pixelBoxX + pixelOfInterestX >= 0 &&
                pixelBoxX + pixelOfInterestX < (*width)) {
              int pixelBoxIndexY = pixelOfInterestY + pixelBoxY;
              int pixelBoxIndexX = pixelOfInterestX + pixelBoxX;
              int idx = pixelBoxIndexY * (*width) + pixelBoxIndexX;
              maxValBlue = max(maxValBlue, inputBlue[idx]);
              maxValGreen = max(maxValGreen, inputGreen[idx]);
              maxValRed = max(maxValRed, inputRed[idx]);
            }
          }
        }
        int idx = pixelOfInterestY * (*width) + pixelOfInterestX;
        outputBlue[idx] = maxValBlue;
        outputGreen[idx] = maxValGreen;
        outputRed[idx] = maxValRed;
      }
    }
  }
}
__kernel void bgr_to_ycbcr(__global uchar* B, __global uchar* G, __global uchar* R, __global uchar* Y, __global uchar* Cb, __global uchar* Cr) {
    int idx = get_global_id(0);
    uchar b = B[idx];
    uchar g = G[idx];
    uchar r = R[idx];

    Y[idx] = 0.299f * r + 0.587f * g + 0.114f * b;
    Cb[idx] = 128.0f - 0.168736f * r - 0.331264f * g + 0.5f * b;
    Cr[idx] = 128.0f + 0.5f * r - 0.418688f * g - 0.081312f * b;
}
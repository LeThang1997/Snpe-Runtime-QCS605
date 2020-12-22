

#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

unsigned char* load_image_yuv(const char* filename, int w, int h);
int save_image_yuv(const char *filename,const unsigned char *YUV, size_t y_stride, size_t uv_stride);
unsigned char* resize_image_yuv(  unsigned char* buf_src, int src_w, int src_h, int dest_w, int dest_h);
unsigned char* convert_yuv2bgr( unsigned char* buf_src, int src_w, int src_h, int dest_w, int dest_h );
int save_image_bgr(unsigned char* buf_src, int src_w, int src_h, const std::string& dest_file);
int save_yuv2bgr(unsigned char* buf_src, int src_w, int src_h, int dest_w, int dest_h, const std::string& dest_file);
int save_yuv2png(unsigned char* buf_src, int src_w, int src_h, int dest_w, int dest_h, const std::string& dest_file);
unsigned char* nv12_nearest_scale(uint8_t* __restrict src, int srcWidth, int srcHeight, int dstWidth, int dstHeight);
unsigned char* convertYUY420ToNV21(unsigned char* data_yuv420, int w, int h);
#endif /* IMAGEPROCESSING_H */
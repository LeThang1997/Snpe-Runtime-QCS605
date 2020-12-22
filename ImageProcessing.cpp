
/******************************************************************************** 
 Copyright (C) 2020, LE MANH THANG. All rights reserved.
 Module: main.cpp
 Author: LE MANH THANG
 Created: 20/03/1997
 Description: 
********************************************************************************/

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <float.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <limits.h>
#include "ImageProcessing.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void *xcalloc(size_t nmemb , size_t size)
{
    void *ptr = calloc(nmemb, size);
    if(!ptr)
    {
        printf("malloc error \n");
        exit(1);
    }
    return ptr;
}


unsigned char* load_image_yuv(const char* filename, int w, int h)
{
    unsigned char* data;
    FILE        *yuv;
    yuv = fopen(filename,"rb");
    data = (unsigned char*)malloc(w * h *1.5 * sizeof(unsigned char));
    fread(data, sizeof(unsigned char), (w * h) * 1.5, yuv);
    fclose(yuv);
    return data;
}


int save_image_yuv(const char *filename, const unsigned char *YUV, size_t y_stride, size_t uv_stride)
{
	FILE *fp = fopen(filename, "wb");
	if(!fp)
	{
		perror("Error opening yuv image for write");
		return 1;
	}
	fwrite(YUV, 1, y_stride*uv_stride*3/2, fp);	
	fclose(fp);
	return 0;
}


unsigned char* convert_yuv2bgr( unsigned char* buf_src, int src_w, int src_h, int dest_w, int dest_h )
{
    cv::Mat nv_21_mat = cv::Mat(src_h * 1.5, src_w, CV_8UC1, buf_src);
    cv::Mat bgr_mat;
    cv::cvtColor(nv_21_mat, bgr_mat, cv::COLOR_YUV2BGR_NV21);
    cv::Mat bgr_resize;
    cv::resize(bgr_mat, bgr_resize, cv::Size(dest_w, dest_h));
    cv::Mat flat = bgr_resize.reshape(1, bgr_resize.total()*bgr_resize.channels());
    std::vector<unsigned char> vec = bgr_resize.isContinuous()? flat : flat.clone();
    //printf("Size yuv2bgr: %d\n", vec.size());
    unsigned char *bgr = &*vec.begin();
    return bgr;
}

int save_yuv2bgr(unsigned char* buf_src, int src_w, int src_h, int dest_w, int dest_h, const std::string& dest_file)
{
    cv::Mat nv_21_mat = cv::Mat(src_h * 1.5, src_w, CV_8UC1, buf_src);
    cv::Mat bgr_mat;
    cv::cvtColor(nv_21_mat, bgr_mat, cv::COLOR_YUV2BGR_NV21);
    cv::Mat bgr_resize;
    cv::resize(bgr_mat, bgr_resize, cv::Size(dest_w, dest_h));
    uchar* buffer = bgr_resize.data;
    std::ofstream outfile (dest_file, std::ios::out | std::ios::binary);
    outfile.write ((char*)buffer, bgr_resize.total() * bgr_resize.elemSize());
    //printf("Size bgr: %d\n", bgr_resize.total() * bgr_resize.elemSize());
    outfile.close();
    return 0;
}
int save_image_bgr( unsigned char* buf_src, int src_w, int src_h, const std::string& dest_file)
{
    std::ofstream outfile (dest_file, std::ios::out | std::ios::binary);
    outfile.write ((char*)buf_src, src_w * src_h * 3);
    outfile.close();
    return 0;
}

int save_yuv2png(unsigned char* buf_src, int src_w, int src_h, int dest_w, int dest_h, const std::string& dest_file)
{
    cv::Mat nv_21_mat = cv::Mat(src_h * 1.5, src_w, CV_8UC1, buf_src);
    cv::Mat bgr_mat;
    cv::cvtColor(nv_21_mat, bgr_mat, cv::COLOR_YUV2RGB_NV21);
    cv::Mat bgr_resize;
    cv::resize(bgr_mat, bgr_resize, cv::Size(dest_w, dest_h));
    cv::imwrite(dest_file, bgr_resize);
    return 0;
}

unsigned char* nv12_nearest_scale(uint8_t* __restrict src, int srcWidth, int srcHeight, int dstWidth, int dstHeight)      //restrict keyword is for compiler to optimize program
{
    unsigned char* dst = (unsigned char*)malloc(dstWidth * dstHeight * 3 /2);
    int sw = srcWidth;  //register keyword is for local var to accelorate 
    int sh = srcHeight;
    int dw = dstWidth;
    int dh = dstHeight;
    int y, x;
    unsigned long int srcy, srcx, src_index;
    unsigned long int xrIntFloat_16 = (sw << 16) / dw + 1; //better than float division
    unsigned long int yrIntFloat_16 = (sh << 16) / dh + 1;

    uint8_t* dst_uv = dst + dh * dw; //memory start pointer of dest uv
    uint8_t* src_uv = src + sh * sw; //memory start pointer of source uv
    uint8_t* dst_uv_yScanline;
    uint8_t* src_uv_yScanline;
    uint8_t* dst_y_slice = dst; //memory start pointer of dest y
    uint8_t* src_y_slice;
    uint8_t* sp;
    uint8_t* dp;
 
    for (y = 0; y < (dh & ~7); ++y)  //'dh & ~7' is to generate faster assembly code
    {
        srcy = (y * yrIntFloat_16) >> 16;
        src_y_slice = src + srcy * sw;

        if((y & 1) == 0)
        {
            dst_uv_yScanline = dst_uv + (y / 2) * dw;
            src_uv_yScanline = src_uv + (srcy / 2) * sw;
        }

        for(x = 0; x < (dw & ~7); ++x)
        {
            srcx = (x * xrIntFloat_16) >> 16;
            dst_y_slice[x] = src_y_slice[srcx];

            if((y & 1) == 0) //y is even
            {
                if((x & 1) == 0) //x is even
                {
                    src_index = (srcx / 2) * 2;
            
                    sp = dst_uv_yScanline + x;
                    dp = src_uv_yScanline + src_index;
                    *sp = *dp;
                    ++sp;
                    ++dp;
                    *sp = *dp;
                }
             }
         }
         dst_y_slice += dw;
    }
    return dst;
}

unsigned char* convertYUY420ToNV21(unsigned char* data_yuv420, int w, int h) 
{
    unsigned char* data_yuv_N21 = (unsigned char*)malloc(w * h * 3 /2);
    int idx = (int)(strlen((char*)data_yuv_N21) * (2.0f/  3.0f));
    int j = idx;
    int chroma_plane_end = (int) (idx + ((strlen((char*)data_yuv_N21) - idx) / 2));

    for (int i = idx; i < chroma_plane_end; i++) {
        data_yuv_N21[j] = data_yuv420[i];
        j += 2;
    }
    j = idx + 1;
    for (int i = chroma_plane_end; i < strlen((char*)data_yuv_N21) ; i++) {
        data_yuv_N21[j] = data_yuv420[i];
        j += 2;
    }
    return data_yuv_N21;
}

void nv21_bgr(unsigned int width , unsigned int height , unsigned char *yuyv , unsigned char *rgb)
{
    const int nv_start = width * height ;
    unsigned int  i, j, index = 0, rgb_index = 0;
    unsigned char y, u, v;
    int r, g, b, nv_index = 0;
    for(i = 0; i <  height ; i++)
    {
		for(j = 0; j < width; j ++){
			//nv_index = (rgb_index / 2 - width / 2 * ((i + 1) / 2)) * 2;
			nv_index = i / 2  * width + j - j % 2;
 
			y = yuyv[rgb_index];
			u = yuyv[nv_start + nv_index ];
			v = yuyv[nv_start + nv_index + 1];
			
			r = y + (140 * (v-128))/100;  //r
			g = y - (34 * (u-128))/100 - (71 * (v-128))/100; //g
			b = y + (177 * (u-128))/100; //b
				
			if(r > 255)   r = 255;
			if(g > 255)   g = 255;
			if(b > 255)   b = 255;
       		if(r < 0)     r = 0;
			if(g < 0)     g = 0;
			if(b < 0)     b = 0;
			
			index = rgb_index % width + (height - i - 1) * width;
			rgb[index * 3+0] = b;
			rgb[index * 3+1] = g;
			rgb[index * 3+2] = r;
			rgb_index++;
		}
    }
}


void ProcessInputWithFloatModel(uint8_t* input, float* buffer, int wanted_input_width, int wanted_input_height, int wanted_input_channels) 
{
  for (int y = 0; y < wanted_input_height; ++y) 
  {
    float* out_row = buffer + (y * wanted_input_width * wanted_input_channels);
    for (int x = 0; x < wanted_input_width; ++x) 
	{
      uint8_t* input_pixel = input + (y * wanted_input_width * wanted_input_channels) + (x * wanted_input_channels);
      float* out_pixel = out_row + (x * wanted_input_channels);
      for (int c = 0; c < wanted_input_channels; ++c) 
	  {
        out_pixel[c] = input_pixel[c] / 255.0f;
      }
    }
  }
}
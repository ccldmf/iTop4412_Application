/**
 * @brief
 * @file     main.cpp
 * @author   cc
 * @version
 * @date     Mon 15 Jan 2018 21:43:55 CST
 * @mail     13568859409@163.com
 * @note
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include "camera.h"

#define DEV_NAME_LENGTH     50
#define NUM_FRAM            200

int main(int argc, char ** argv)
    {
    char *dev_name = "/dev/video0";//ITU
    FILE * outf = 0;
    unsigned int image_size;

    int width=640;
    int height=480;
    unsigned int writesize=0;
    unsigned char image[width*height*2];

    outf = fopen("out.yuv", "wb");
    Camera *camera;

    camera=new Camera(dev_name,width,height);
    //打开设备
    if(!camera->OpenDevice())
        {
        printf("OpenDevice error\n");
        return -1;
        }
    //获得一帧图像的大小
    image_size=camera->getImageSize();
    for(int i=0;i<NUM_FRAM;i++)
        {
        //获得一帧图像
        if(!camera->GetBuffer(image))
            {
            printf("GetBuffer error\n");
            break;
            }
        //将数据写到文件中
        writesize=fwrite(image,1,image_size,outf);
        if(writesize < 0)
            {
            printf("write image error\n");
            break;
            }
        }
    fclose(outf);
    printf("Close out.yuv ok\n");
    //关闭设备
    camera->CloseDevice();
    //关闭存放视频文件
    return 0;
    }


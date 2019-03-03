/**
 * @brief
 * @file     camera.h
 * @author   cc
 * @version
 * @date     Mon 15 Jan 2018 21:44:50 CST
 * CST
 * @mail     13568859409@163.com
 * @note
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <sys/types.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
    void * start;
    size_t length;//buffer's length is different from cap_image_size
};

class Camera
{
public:
    //类相关参数初始化
    Camera(char *DEV_NAME,int w,int h);
    ~Camera();
    bool OpenDevice();//打开设备
    void CloseDevice();
    bool GetBuffer(unsigned char *image);//获取数据流
    unsigned int getImageSize();

private:
    char dev_name[50];
    int fd;
    int width;
    int height;
    struct buffer * buffers ;
    unsigned int n_buffers ;
    unsigned int cap_image_size ;//to keep the real image size!!

    bool init_device(void);
    bool init_mmap(void);
    void uninit_device(void);
    bool start_capturing(void);
    void stop_capturing(void);
    int read_frame(unsigned char *image);
    void close_device(void);
    bool open_device(void);

    int xioctl(int fd, int request, void * arg);
};

#endif // CAMERA_H


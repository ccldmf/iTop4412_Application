arm-none-linux-gnueabi-gcc -o lcd4412 lcd_framebuffer.c lcd-test.c  -static
cp lcd4412 /home/cc/iTop4412/busybox/system/bin

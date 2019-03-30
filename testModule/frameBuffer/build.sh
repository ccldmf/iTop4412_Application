arm-none-linux-gnueabi-gcc -o lcd4412 ../../module/frameBuffer/lcd_framebuffer.c ../../module/frameBuffer/drawGraphics.c lcd-test.c  -static
cp lcd4412 /home/cc/share/tmp

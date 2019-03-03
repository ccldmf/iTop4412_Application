arm-none-linux-gnueabi-gcc -o trfid rfid-rc522.c rfid-test.c  -static
cp trfid /home/cc/iTop4412/busybox/system/bin

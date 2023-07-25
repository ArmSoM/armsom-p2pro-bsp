#version number
rk3308_linux5.10_release_v1.0.3_20221220.xml

#First compilation
./build.sh lunch
choice 2
./build.sh 

#build debain
RELEASE=bullseye ARCH=arm64 ./mk-base-debian.sh
VERSION=debug ARCH=arm64 ./mk-rootfs-bullseye.sh
./mk-image.sh

#Use wifi under Debian
insmod bcmdhd.ko firmware_path=/system/etc/firmware/ nvram_path=/system/etc/firmware/
brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download  --tosleep  200000 --baudrate 1500000 --patchram  /system/etc/firmware/BCM4345C0.hcd /dev/ttyS4 &



## Compile environment

* Hardware requirements: 64-bit system with more than 40 GB disk space. If you do multiple builds, you will need more hard disk space.

* Software requirements: Ubuntu 20.04 system:

### 1、Install libraries and toolsets：

```shell
sudo apt-get install git ssh make gcc libssl-dev liblz4-tool expect g++ patchelf chrpath gawk texinfo chrpath diffstat binfmt-support qemu-user-static live-build bison flex fakeroot cmake gcc-multilib g++-multilib unzip device-tree-compiler ncurses-dev libgucharmap-2-90-dev bzip2 expat gpgv2 cpp-aarch64-linux-gnu time mtd-utils
```

### 2、Check and upgrade software packages：

If there is a compilation error in the future, it may be due to the incorrect version of the compilation tool, the following two examples may be updated

- Check the make version (make 4.0 and above required)
```
make -v
GNU Make 4.2
Built for x86_64-pc-linux-gnu
```

- Upgrade make version
```
git clone https://github.com/mirror/make.git
cd make
git checkout 4.2
git am $BUILDROOT_DIR/package/make/*.patch
autoreconf -f -i
./configure
make make -j8
sudo install -m 0755 make /usr/bin/make
```


- Check the lz4 version (lz4 1.7.3 or later is required).
```
lz4 -v
*** LZ4 command line interface 64-bits v1.9.4, by Yann Collet ***
refusing to read from a console
```

- Upgrade lz4 version
```
git clone https://github.com/lz4/lz4.git
cd lz4
make
sudo make install
sudo install -m 0755 lz4 /usr/bin/lz4
```

## Configuration framework description

Copy the SDK to your working directory under ubuntu

```
git clone https://github.com/ArmSoM/armsom-p2pro-bsp
```

### 1、 compilation command

* View compilation command
```
./build.sh -h
```

* Select the configuration file for the development board
```
./build.sh lunch
You're building on Linux
Lunch menu...pick a combo:

0. default BoardConfig.mk
1. BoardConfig.mk
2. BoardConfig_32bit.mk
3. BoardConfig_rk3308_bpi-p2_pro.mk
4. BoardConfig_rk3308b_32bit.mk
5. BoardConfig_rk3308b_64bit.mk
6. BoardConfig_rk3308bs_32bit.mk
7. BoardConfig_rk3308bs_64bit.mk
8. BoardConfig_rk3308h_32bit.mk
9. BoardConfig_rk3308hs_32bit.mk
Which would you like? [0]: 3
```

* Full automatic compilation
```
./build.sh all # Only compile module code (u-Boot, kernel, Rootfs, Recovery)

# Need to run./mkfirmware.sh to package the firmware



./build.sh # Compile module code (u-Boot, kernel, Rootfs, Recovery)

# Pack as update.img full upgrade package

# All compilation information is copied and generated into the out directory
```

The default is to compile the Buildroot system. You can specify different rootfs by setting the RK_ROOTFS_SYSTEM.

RK_ROOTFS_SYSTEM You can set three types of systems: buildroot, debian, and yocto.

For example, the command to generate debian is as follows:

```
export RK_ROOTFS_SYSTEM=debian
./build.sh
```

* module compilation
```
./build.sh uboot
./build.sh kernel
./build.sh recovery
./build.sh rootfs
...
```

### 2、 kernel Build Command
To customize the kernel, run the following command
```
cd kernel
make ARCH=arm64 rk3308_linux_defconfig
make ARCH=arm64 menuconfig
make ARCH=arm savedefconfig
cp .config arch/arm/configs/rk3308_linux_defconfig
```

## Use development board
There is a [wiki](http://wiki.armsom.org/index.php/Getting_Started_with_ArmSoM-p2_pro "悬停显示") documentation on how to use the development board


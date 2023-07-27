#version number

rk3308_linux5.10_release_v1.0.3_20221220.xml


## SDK编译环境搭建

### 1、安装库和工具集：
```shell
sudo apt-get install git ssh make gcc libssl-dev liblz4-tool expect g++ patchelf chrpath gawk texinfo chrpath diffstat binfmt-support qemu-user-static live-build bison flex fakeroot cmake gcc-multilib g++-multilib unzip device-tree-compiler ncurses-dev libgucharmap-2-90-dev bzip2 expat gpgv2 cpp-aarch64-linux-gnu time mtd-utils
```

### 2、拷贝至工作目录

```
git clone https://github.com/ArmSoM/armsom-p2pro-bsp
```

###3、 SDK编译命令
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

###4、全自动编译
```
./build.sh all # 只编译模块代码（u-Boot，kernel，Rootfs，Recovery）
# 需要再执⾏./mkfirmware.sh 进⾏固件打包
./build.sh # 编译模块代码（u-Boot，kernel，Rootfs，Recovery）
# 打包成update.img完整升级包
# 所有编译信息复制和⽣成到out⽬录下
```
默认是 Buildroot，可以通过设置坏境变量 RK_ROOTFS_SYSTEM 指定不同 rootfs。 

RK_ROOTFS_SYSTEM ⽬前可设定三种系统：buildroot、debian、 yocto 。

比如需要生成debian的命令如下：

```
export RK_ROOTFS_SYSTEM=debian
./build.sh
```

###5、模块编译

```
./build.sh uboot
./build.sh kernel
./build.sh recovery
./build.sh rootfs
...
```

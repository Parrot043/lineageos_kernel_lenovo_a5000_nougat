#!/bin/bash

OUT_DIR="out"
KERNEL_DIR=$PWD
KERN_IMG=${OUT_DIR}/arch/arm/boot/zImage
BUILD_START=$(date +"%s")

#Set Color
blue='\033[0;34m'
green='\033[0;32m'
yellow='\033[0;33m'
red='\033[0;31m'
nocol='\033[0m'
lightgray='\033[0;37m'

# Set configs
mkdir ${OUT_DIR}
export CROSS_COMPILE=~/toolchains/arm-eabi-4.9/bin/arm-eabi-
export JAVA_HOME=/usr/lib/jvm/java-7-oracle
export USE_CCACHE=1
export ARCH=arm
export SUBARCH=arm
export KBUILD_BUILD_USER=bulkin042
export KBUILD_BUILD_HOST=github
export TARGET_BUILD_VARIANT=adder

compile_kernel ()
{
echo -e "$green Clean old files"
rm ${OUT_DIR}/arch/arm/boot/Image
rm ${OUT_DIR}/arch/arm/boot/zImage

echo -e "$cyan Make DefConfig"
make O=${OUT_DIR} alps_defconfig
echo -e "$green Build kernel"
make O=${OUT_DIR} -j$(grep -c ^processor /proc/cpuinfo)

if ! [ -a $KERN_IMG ];
then
echo -e "$red Kernel Compilation failed! Fix the errors! $nocol"
exit 1
fi
}

case $1 in
clean)
make ARCH=arm -j$(grep -c ^processor /proc/cpuinfo) clean mrproper
;;
*)
compile_kernel
;;
esac
cp ${OUT_DIR}/arch/arm/boot/zImage  ${KERNEL_DIR}/arch/arm/boot/zImage
BUILD_END=$(date +"%s")
DIFF=$(($BUILD_END - $BUILD_START))
echo -e "$yellow Build completed in $(($DIFF / 60)) minute(s) and $(($DIFF % 60)) seconds.$nocol"
echo -e "$yellow Enjoy Kernel build.$nocol"

#!/bin/sh

# I put all my dev stuff in here
export DEV_PREFIX=$HOME/src

# Don't forget to adjust this to your NDK path
export ANDROID_NDK=${DEV_PREFIX}/crystax-ndk

export CROSS_COMPILE=arm-linux-androideabi
export ANDROID_PREFIX=${ANDROID_NDK}/prebuilt/darwin-x86
export SYSROOT=${ANDROID_NDK}/platforms/android-18/arch-arm
export CROSS_PATH=${DEV_PREFIX}/android18/bin/${CROSS_COMPILE}

# Non-exhaustive lists of compiler + binutils
# Depending on what you compile, you might need more binutils than that
export CPP=${CROSS_PATH}-cpp
export AR=${CROSS_PATH}-ar
export AS=${CROSS_PATH}-as
export NM=${CROSS_PATH}-nm
export CC=${CROSS_PATH}-gcc
export CXX=${CROSS_PATH}-g++
export LD=${CROSS_PATH}-ld
export RANLIB=${CROSS_PATH}-ranlib

# Don't mix up .pc files from your host and build target
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig

# export CFLAGS="${CFLAGS} --sysroot=${SYSROOT} -I${SYSROOT}/usr/include -I${ANDROID_PREFIX}/include -I${DEV_PREFIX}/android/bionic"
export CFLAGS="${CFLAGS} --sysroot=${SYSROOT}"
export CPPFLAGS="${CFLAGS}"
export CXXFLAGS="${CFLAGS}"
# export LDFLAGS="${LDFLAGS} -L${SYSROOT}/usr/lib -L${ANDROID_PREFIX}/lib"

export ac_cv_func_malloc_0_nonnull=yes
export ac_cv_func_realloc_0_nonnull=yes

./configure --build=x86_64-pc-darwin --target=arm-linux-gnueabi --host=arm-linux-gnueabi "$@" # --with-sysroot=${SYSROOT} "$@"

#!/bin/sh
set -xe

KERNEL=./linux/arch/x86/boot/bzImage
ROOTFS=./rootfs/rootfs.cpio.gz

qemu-system-x86_64 \
  -kernel $KERNEL \
  -initrd $ROOTFS \
  -no-reboot \
  -nographic \
  -append 'console=ttyS0 nokaslr panic=-1' \
  -s

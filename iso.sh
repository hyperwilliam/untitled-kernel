#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp $SYSROOT/boot/UntitledOS.kernel isodir/boot/UntitledOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "UntitledOS" {
	multiboot /boot/UntitledOS.kernel
}
EOF
grub-mkrescue -o UntitledOS.iso isodir

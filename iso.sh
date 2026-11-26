#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
mkdir -p isodir/System
mkdir -p isodir/System/Core

cp $SYSROOT/boot/UntitledOS.kernel isodir/System/Core/UntitledOS.kernel
tar -cf isodir/System/Core/Init.img -C initrd . # TODO: make the config options work
# the init ramdisk has to be loaded first
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "UntitledOS" {
	multiboot /System/Core/UntitledOS.kernel
	module /System/Core/Init.img
}
EOF
grub-mkrescue -o UntitledOS.iso isodir # NOTE: make sure not to accidentaly upload the .ISO image, that would be bad.

#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

# TODO: Think of a good file tree and implement it.

cp $SYSROOT/boot/UntitledOS.kernel isodir/boot/UntitledOS.kernel
tar -cf isodir/boot/init.rd -C isofiles . # TODO: make the config options work
# the init ramdisk has to be loaded first
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "UntitledOS" {
	multiboot /boot/UntitledOS.kernel
	module /boot/init.rd
}
EOF
grub-mkrescue -o UntitledOS.iso isodir # NOTE: make sure not to accidentaly upload the .ISO image, that would be bad.

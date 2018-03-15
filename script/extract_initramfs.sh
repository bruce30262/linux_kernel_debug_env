#!/usr/bin/env sh

# remove old initramfs
if [ -d ./initramfs ]
then
    rm -rf ./initramfs
fi
# create initramfs
mkdir -p initramfs
# extract rootfs.cpio to initramfs
cd ./initramfs &&\
cp ../rootfs.cpio ./tmp_rootfs.cpio.gz &&\
gzip -d ./tmp_rootfs.cpio.gz &&\
cpio -idv < ./tmp_rootfs.cpio
# delete tmp_rootfs.cpio
rm ./tmp_rootfs.cpio
echo "Done"

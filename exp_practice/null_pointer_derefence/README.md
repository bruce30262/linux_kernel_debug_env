NULL pointer dereference practice  
ref: [link](http://tacxingxing.com/2018/02/22/linuxkernelexploit-null-dereference/) 

init in rootfs:
```
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs devtmpfs /dev
insmod /root/null.ko
setsid cttyhack setuidgid 0 sh

umount /proc
umount /sys
poweroff -d 0 -f
```

copy `exp` & `null.ko` to `initramfs/root/` and run `mkcpio.sh` to build rootfs.cpio

run.sh:
```
#!/bin/sh                                                                                                                                                                               
qemu-system-x86_64 \
-initrd rootfs.cpio \
-kernel bzImage \
-append 'console=ttyS0 root=/dev/ram oops=panic panic=1' \
-monitor /dev/null \
-m 64M \
--nographic \
-smp cores=1,threads=1 \
-cpu qemu64 \
-s
```
no SMEP, KASLR

to exploit:
1. run `sysctl -w vm.mmap_min_addr="0"` as root
2. run `setuidgid 1000 sh` to switch to uid 1000
3. run `/root/exp` to get root shell

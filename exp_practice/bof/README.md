BOF practice  
linux kernel version: 4.13.0-32-generic

init in rootfs:
```
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs devtmpfs /dev
insmod /root/bof.ko
setsid cttyhack setuidgid 1000 sh

umount /proc
umount /sys
poweroff -d 0 -f
```

copy `exp` & `bof.ko` to `initramfs/root/` and run `mkcpio.sh` to build rootfs.cpio

run.sh:
```
#!/bin/sh                                                                                                                                                                               
qemu-system-x86_64 \
-initrd rootfs.cpio \
-kernel bzImage \
-append 'console=ttyS0 root=/dev/ram oops=panic panic=1 kaslr' \
-monitor /dev/null \
-m 64M \
--nographic \
-smp cores=1,threads=1 \
-cpu qemu64,+smep,+smap \
-s
```

to exploit, run `/root/exp` to get root shell

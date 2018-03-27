#ifndef IOCTL_H
#define IOCTL_H

#include <linux/ioctl.h>

#define UAF_IOCTL_MAGIC 0x44
#define UAF_IOCTL_MALLOC _IOR(UAF_IOCTL_MAGIC, 0, int)
#define UAF_IOCTL_FREE   _IO(UAF_IOCTL_MAGIC, 1)

#endif

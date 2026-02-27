#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/uaccess.h>
SYSCALL_DEFINE2(myhello, char __user *, buf, int, len)
{
    const char *message="hello world!";
    size_t message_len=strlen(message)+1;
    if(len<message_len)
    {
        return -EINVAL;
    }
    if(copy_to_user(buf,message,message_len))
    {
        return -EFAULT;
    }
    return 0;
}
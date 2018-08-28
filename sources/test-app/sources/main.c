#include <syscalls.h>

int main(int argc, const char ** argv)
{
    
    for(int i = 0; i < 10; i++)
    {
        syscall_print("Hello from userspace!\n");
    }
    syscall_exit(0);
}
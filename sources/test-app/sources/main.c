#include <syscalls.h>

int main(int argc, const char ** argv)
{
    syscall_print("Hello from userspace!\n");
    syscall_exit(0);
}
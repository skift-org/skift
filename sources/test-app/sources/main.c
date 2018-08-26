#include <syscalls.h>

int main(int argc, const char ** argv)
{
    syscall_print("Hello world!\n");
    syscall_exit(0);
}
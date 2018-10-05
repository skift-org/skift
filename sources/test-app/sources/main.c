#include <skift/io.h>
#include <skift/process.h>

int main(int argc, const char **argv)
{
    sk_io_print("Hello from userspace!");
    sk_process_exit(0);
    return 0;
}
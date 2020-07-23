
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    // do a page fault
    asm("int $1");

    return 0;
}

double tan(double x)
{
    float out;
    float _x = x;
    float one;
    asm volatile(
        "fld %2\n"
        "fptan\n"
        "fstp %1\n"
        "fstp %0\n"
        : "=m"(out), "=m"(one)
        : "m"(_x));

    return out;
}

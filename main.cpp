#include <karm-base/func.h>

int fi(int a)
{
    return a + 1;
}

int main()
{
    Karm::Base::Func<int(int)> f = fi;
}

#include <libutils/Variant.h>

#include "tests/Driver.h"

using Number = Utils::Variant<int, double>;

TEST(variant)
{
    Number nbr;

    nbr = 5;
    Assert::is_true(nbr.is<int>());
    Assert::is_false(nbr.is<double>());

    nbr = 5.;
    Assert::is_true(nbr.is<double>());
    Assert::is_false(nbr.is<int>());
}
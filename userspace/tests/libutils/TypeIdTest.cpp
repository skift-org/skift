#include <libutils/TypeId.h>

#include "tests/Driver.h"

TEST(typeid)
{
    Assert::not_equal(GetTypeId<int>(), GetTypeId<int &>());
    Assert::equal(GetTypeId<int>(), GetTypeId<int>());
}
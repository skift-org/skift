#include <libutils/Variant.h>

#include "libutils/Assert.h"
#include "tests/Driver.h"

using Number = Utils::Variant<int, double>;

TEST(variant_default_constructor)
{
    Number nbr;

    Assert::is_true(nbr.is<int>());
    Assert::is_false(nbr.is<double>());

    Assert::equal(nbr.get<int>(), 0);
}

TEST(variant_is_method)
{
    Number nbr;

    nbr = 5;
    Assert::is_true(nbr.is<int>());
    Assert::is_false(nbr.is<double>());
    
    nbr = 6.;
    Assert::is_true(nbr.is<double>());
    Assert::is_false(nbr.is<int>());
}

TEST(variant_get_method)
{
    Number nbr;

    nbr = 5;
    Assert::equal(nbr.get<int>(), 5);
    
    nbr = 6.;
    Assert::equal(nbr.get<double>(), 6);
}

TEST(variant_with_method)
{
    Number nbr;

    nbr = 5;
    nbr.with<double>([](double &) {
        Assert::not_reached();
    });

    nbr.with<int>([](int &v) {
        Assert::equal(v, 5);
    });

    nbr = 6.;
    nbr.with<int>([](int &) {
        Assert::not_reached();
    });

    nbr.with<double>([](double &v) {
        Assert::equal(v, 6.);
    });

    Assert::equal(nbr.get<double>(), 6);
}

TEST(variant_visit)
{
    Number nbr5{5};
    Number nbr6{6.0};

    bool has_visited_int = false;
    bool has_visited_double = false;
    
    Utils::Visitor visitor{
        [&](int &value) { Assert::equal(value, 5); has_visited_int = true;},
        [&](double &value) { Assert::equal(value, 6.); has_visited_double = true;},
    };

    nbr5.visit(visitor);
    nbr6.visit(visitor);

    Assert::is_true(has_visited_int);
    Assert::is_true(has_visited_double);
}

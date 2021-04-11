#pragma once

#include <libinjection/Entity.h>
#include <libutils/String.h>

struct NumericType :
    public virtual Injection::Entity
{
    virtual int func() = 0;
};

struct StringType :
    public virtual Injection::Entity
{
    virtual String string() = 0;
};

struct Type42 :
    public NumericType
{
    Type42(Injection::Context &) {}

    int func() override { return 42; }
};

struct Type52 :
    public NumericType
{
    Type52(Injection::Context &) {}

    int func() override { return 52; }
};

struct Type5Apple :
    public NumericType,
    public StringType
{
    Type5Apple(Injection::Context &) {}

    int func() override { return 5; }
    String string() { return "apple"; }
};

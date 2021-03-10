#pragma once

enum class AdoptTag
{
    __value
};

enum class CopyTag
{
    __value
};

enum class WrapTag
{
    __value
};

constexpr auto ADOPT = AdoptTag::__value;
constexpr auto COPY = CopyTag::__value;
constexpr auto WRAP = WrapTag::__value;

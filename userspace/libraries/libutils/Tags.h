#pragma once

namespace Utils
{

enum struct AdoptTag
{
    __value
};

enum struct CopyTag
{
    __value
};

enum struct WrapTag
{
    __value
};
enum struct NoneTag
{
    __value
};

/* The object should take the ownership of the memory */
constexpr auto ADOPT = AdoptTag::__value;

/* The object should make a copy of the memory */
constexpr auto COPY = CopyTag::__value;

/* The object should wrap the memory without doing a copy
   nor taking the ownership */
constexpr auto WRAP = WrapTag::__value;

/* The object should be empty initialized */
constexpr auto NONE = NoneTag::__value;

} // namespace Utils

#pragma once
#include <assert.h>

#ifndef assert_false
#    define assert_false(__expr) ((!__expr) ? (void)(0) : assert_failed(#    __expr " is not false", __FILE__, __FUNCTION__, __LINE__))
#endif
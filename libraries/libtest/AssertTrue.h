#pragma once
#include <assert.h>

#ifndef assert_true
#    define assert_true(__expr) ((__expr) ? (void)(0) : assert_failed(#    __expr " is not true", __FILE__, __FUNCTION__, __LINE__))
#endif